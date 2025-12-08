/*_ time.c   Sun Apr 16 1989   Modified by: Walter Bright */
/* Copyright (C) 1986-1991 by Walter Bright		*/
/* All Rights Reserved					*/
/* Written by Walter Bright				*/

#include	<stdio.h>
#include	<time.h>
#include	<errno.h>
#include	<string.h>

#if _MT
#include	"mt.h"
#endif

/******************************
 * Convert broken-down time into a string of the form:
 *	Wed Apr 16 15:14:03 1986\n\0
 * Returns:
 *	A pointer to that string (statically allocated).
 *	The string is overwritten by each call to asctime() or ctime().
 */

#if Aasctime

static void _near _pascal decput(char near *s,int n)
{
	*s = (n / 10) + '0';
	*(s + 1) = (n % 10) + '0';
}


char *asctime(const struct tm *ptm)
{
	static char months[12*3+1] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	static char days[7*3+1] = "SunMonTueWedThuFriSat";

#if _MT
	char *string;

	string = _getthreaddata()->t_asctime;
	sprintf(string,
		"%.3s %.3s %2d %2d:%02d:%02d %4d\n",
		days + ptm->tm_wday * 3,months + ptm->tm_mon * 3,ptm->tm_mday,
		ptm->tm_hour,ptm->tm_min,ptm->tm_sec,1900 + ptm->tm_year);
#else
	static char string[26] = "ddd mmm dd hh:mm:ss 19yy\n";

	memcpy(string,&days[ptm->tm_wday * 3],3);
	memcpy(&string[4],&months[ptm->tm_mon * 3],3);
	string[8] = (ptm->tm_mday >= 10) ? (ptm->tm_mday / 10) + '0' : ' ';
	string[9] = (ptm->tm_mday % 10) + '0';
	decput(&string[11],ptm->tm_hour);
	decput(&string[14],ptm->tm_min);
	decput(&string[17],ptm->tm_sec);
	decput(&string[22],ptm->tm_year);
#endif
	return string;
}

#endif

#if Actime

/***************************
 * Convert time from time_t to string returned by asctime().
 */

char *ctime(const time_t *pt)
{
	return asctime(localtime(pt));
}

#endif

/**************************
 * # of days in year at start of month
 */

extern short _mdays[13];		/* from TIME2.ASM		*/

#define SECSPERHOUR	(60*60)
#define SECSPERDAY	(SECSPERHOUR*24L)

/**************************
 * Convert from time_t to broken-down time.
 * Returns:
 *	pointer to broken-down time struct, which is static and is
 *	overwritten by each call to localtime().
 */

#if Aloctime

#if M_UNIX || M_XENIX
/***********
For UNIX we have to parse the environment variable TZ for daylight savings
time.

It appears that TIMEOFFSET is based on PDT from GMT.  PDT is GMT - 7 hours.  
Keep this in mind when looking at the following code.  /etc/TIMEZONE and
TIMEZONE(F) are also useful references.

Format of TZ environment variable could be as simple as:
TZ='PST8'   Which means name of the zone is 'PST' which is 8 hours west of
GMT with no daylight savings time.

More complicated values are:
TZ='PST8PDT' Means PST is 8 hours west, PDT is name for daylight time, which
follows current US law for the date and time of change and is 1 hour less 
than standard time.

TZ='PST8:00:00PDT7:00:00' Means the same as the previous example.

TZ='PST8:00:00PDT7:00:00;117/2:00:00,299/2:00:00' Means same as previous 
except change over to daylight time occurs on 117th day (0 based -- Jan 1 is
day 0) at 2:00:00 AM.  Change over to standard time occurs on 299th day at
2:00:00 AM.  Note that for the southern hemisphere start of daylight time
will be at a date greater than end of daylight time.  Also note that leap
year is never counted.  That is March 1 is always day 59 (0 based) or J60
even on leap years.

Start and end dates for daylight savings time have other variations:
J117/2:00:00 would be a 1 based calender -- Jan 1 is day 1.

W15.6  Means week 15 (1 <= n <= 53) day of week 6 (Sunday == 0 -- 0 <= d <= 6).
Week 53 always means the last week containing day 'd' even if there are
actually only 52 weeks containing day d.

M4.5.6 Means day 6 (Sunday) of week 5 (1 <= n <= 5) month 4 (April).  Week
5 of a month is always the last week containing day 'd', whether there are 
actually 4 or 5 weeks containing day d.
*********************/

/* First Sunday in April, last Sunday in October.*/
#define US_RULE ";M4.1.0/2,M10.5.0"

#include <ctype.h>
#include <stdlib.h>

static char * _near _pascal next_number(char *p)
{
    /* Advance past this number (if it exists). */
    while(*p && (*p == '+' || *p == '-' || isdigit(*p)))
	p++;
    /* Find the next number in the string, but not past a ';' */
    while(*p && *p != ';' && *++p != '+' && *p != '-' && !isdigit(*p))
	;
    return p;
}

/*****************
Return the number of seconds different from GMT that we are.
*/
static time_t _near _pascal gmt_offset(char *tz_ptr)
{
    time_t offset = 0;

    if(tz_ptr && *tz_ptr)
    {
	tz_ptr = next_number(tz_ptr);
	if(*tz_ptr)
	{
	    int sign = 1;

	    offset = atol(tz_ptr) * SECSPERHOUR;
	    if(offset < 0)
		sign = -1;

	    tz_ptr = next_number(tz_ptr);	/* Get minutes offset. */
	    if(*tz_ptr && *tz_ptr != ';')
	    {
		offset += sign * atol(tz_ptr) * 60;
		tz_ptr = next_number(tz_ptr);
		if(*tz_ptr && *tz_ptr != ';')
		    offset += sign * atol(tz_ptr);
	    }
	}
    }
    return SECSPERHOUR * 7 - offset;
}

/********************
Return the number of seconds since the beginning of the year that this
daylight savings time expression represents.  The first character pointed to
by p should be either a ';' or a ',' and is to be ignored.  d_offset is the
day of the week that Jan 1 of this year falls on.  This should have been
'fudged' before reaching here if it is leap year and it is after Feb 28.
********************/
static long _near _pascal dst_time(char *p, int d_offset)
{
    long int month, ret_val;
    if(isdigit(*++p))	/* expression in 0 based days? */
    {
	ret_val = 0;
	goto j_case;
    }
    else
	ret_val = -1;

    switch(*p++)
    {
    case 'J':
j_case:
	ret_val += atol(p);	/* Number of days into the year. */
	ret_val *= SECSPERDAY;
	break;
    case 'W':
	ret_val = atol(p) * 7 * SECSPERDAY;
	while(isdigit(*p))
	    p++;
	if(*p == '.')
	    ret_val += atol(p) * SECSPERDAY;
	break;
    case 'M':
	month = atoi(p) - 1;

	/* Get day of week of 1st day of this month. */
	d_offset = (d_offset + _mdays[month]) % 7;
	while(isdigit(*p))
	    p++;
	if(*p++ == '.')
	{
	    int week = atoi(p) - 1;
	    while(isdigit(*p))
		p++;
	    if(*p++ == '.')
	    {
		static char days_in_month[12]=
			/*  J  F  M  A  M  J  J  A  S  O  N  D */
			  {31,28,31,30,31,30,31,31,30,31,30,31};
		int temp, day = atoi(p);
		temp = week * 7 + (7 + day - d_offset) % 7;
		if(temp > days_in_month[month])
		    temp -= 7;
		ret_val = (_mdays[month] + temp) * SECSPERDAY;
	    }
	}
	break;
    }

    if((p = strchr(p,'/')) != 0)
    {
	ret_val += atol(++p) * SECSPERHOUR;
	while(isdigit(*p))
	    p++;
	if(*p == ':')
	{
	    ret_val += atol(++p) * 60;
	    while(isdigit(*p))
		p++;
	    if(*p == ':')
		ret_val += atol(++p);
	}
    }
    return ret_val;
}
/********************
Adjust this struct tm for daylight savings time.
********************/
static void _near _pascal dst_adjust(struct tm *t, char *tz_ptr)
{
    if(tz_ptr)
    {
	time_t dst_offset, std_offset = gmt_offset(tz_ptr);
	tz_ptr = next_number(tz_ptr);	/* Advance to the first number. */

	/* Advance to the daylight time zone name. */
	while(*tz_ptr && !isalpha(*tz_ptr))
	    tz_ptr++;

	if(*tz_ptr == 0)/* If no daylight time zone name then no dst. */
	     t->tm_isdst = 0;
	else
	{   /* Number of seconds from start of year for start/end of dst. */
	    long now, dst_start, dst_end;
		/* Intentional ignore the possibility of leap year. */
	    int jan1_wday = (t->tm_wday - (t->tm_yday % 7) + 7) % 7;

	    now = t->tm_yday;
	    if(now > 58 && (t->tm_year % 4) == 0)/* If a leap year -- day is */
		now--;				/* to ignore leap year. */
	    now *= 24;
	    now += t->tm_hour;	now *= 60;
	    now += t->tm_min;	now *= 60;
	    now += t->tm_sec;

	    while(isalpha(*tz_ptr))
		tz_ptr++;

	    /* Get the offset from GMT for daylight time. */
	    dst_offset = isdigit(*tz_ptr)?
			 gmt_offset(tz_ptr - 1): std_offset - SECSPERHOUR;

	    tz_ptr = strchr(tz_ptr,';');/* If no rule for when to switch */
	    if(!tz_ptr)			/* then use US rule. */
		tz_ptr = US_RULE;

	    dst_start = dst_time(tz_ptr,jan1_wday);
	    tz_ptr = strchr(tz_ptr, ',');
	    if(!tz_ptr)
	    {
		t->tm_isdst = -1;	/* Error -- should be an end time. */
		return;
	    }
	    dst_end = dst_time(tz_ptr,jan1_wday);
	    if(now >= dst_start && now < dst_end ||
		(dst_start > dst_end && (now < dst_end || now >= dst_start)))
	    {
		long delta = std_offset - dst_offset;

		t->tm_isdst = 1;
		t->tm_sec  += delta % 60;	delta /= 60;
		t->tm_min  += delta % 60;	delta /= 60;
		t->tm_hour += delta;
	    }
	    else
		t->tm_isdst = 0;
	}
    }
    else
	t->tm_isdst = -1;	/* Don't know if it is dst or not. */
}
#endif

struct tm *localtime(const time_t *pt)
{
#if _MT
	struct tm bdtime;		/* on stack so we have local copy */
#else
	static struct tm bdtime;
#endif
	time_t t = *pt;
	int i;
#if M_UNIX || M_XENIX
	char *tz_p = getenv("TZ");	/* TZ will give us hours from GMT */
	t += gmt_offset(tz_p);
#endif
	t -= TIMEOFFSET;		/* convert to DOS time		*/

	bdtime.tm_sec = t % 60;			/* seconds 0..59	*/
	bdtime.tm_min = (t / 60) % 60;		/* minutes 0..59	*/
	bdtime.tm_hour = (t / SECSPERHOUR) % 24;/* hour of day 0..23	*/

	t /= SECSPERDAY;		/* t = days since start of 1980	*/
	bdtime.tm_wday = (t + 2) % 7;	/* day of week, 0..6 (Sunday..Saturday) */
	bdtime.tm_year = t / 365 + 1;	/* first guess at year since 1980 */
	do
	{   bdtime.tm_year--;
	    bdtime.tm_yday = t - bdtime.tm_year * 365 -
				((bdtime.tm_year + 3) >> 2);
	}
	while (bdtime.tm_yday < 0);
	bdtime.tm_year += 80;		/* get years since 1900		*/

	for (i = 0; ; i++)
	{	if (i == 0 || (bdtime.tm_year & 3) != 0)
		{	if (bdtime.tm_yday < _mdays[i + 1])
			{	bdtime.tm_mday = bdtime.tm_yday - _mdays[i];
				bdtime.tm_mon = i;
				break;
			}
		}
		else
		{	if (bdtime.tm_yday < _mdays[i + 1] + 1)
			{	bdtime.tm_mday = bdtime.tm_yday -
				    ((i == 1) ? _mdays[i] : _mdays[i] + 1);
				bdtime.tm_mon = i;
				break;
			}
		}
	}
	bdtime.tm_mday++;	/* day of month 1..31	*/
#if M_UNIX || M_XENIX
	dst_adjust(&bdtime, tz_p);
#else
	bdtime.tm_isdst = -1;	/* don't know about daylight savings time */
#endif
#if _MT
	{   /* Copy result to this thread's version of bdtime	*/
	    struct tm *pbdtime = &_getthreaddata()->t_tm;
	    *pbdtime = bdtime;
	    return pbdtime;
	}
#else
	return &bdtime;
#endif
}

#endif

/***********************************
 * Convert from broken-down time into a time_t.
 */

#if Amktime

time_t mktime(tmp)
struct tm *tmp;
{	time_t t;
	unsigned yy;
	unsigned date;

	yy = tmp->tm_year - 80;
	/* if year is before 1980 or would cause an overflow in computing date*/
	if (yy > (32767 - (32767/365 + 3)/4) / 365)
		goto err;
	date = yy * 365 + ((yy + 3) >> 2); /* add day for each leap year */
	date += _mdays[tmp->tm_mon] + tmp->tm_mday - 1;
	if (tmp->tm_mon >= 2 && (yy & 3) == 0)
		date++;			/* this Feb has 29 days in it	*/
	if (date > (unsigned) ((unsigned long)0xFFFFFFFF / (60*60*24L)))
		goto err;
	t = (time_t) date * (time_t) (60*60*24L) +
	    ((tmp->tm_hour * 60L) + tmp->tm_min) * 60L + tmp->tm_sec +
	    TIMEOFFSET;
	*tmp = *localtime(&t);		/* update fields		*/
	return t;

    err:
	return -1;			/* time cannot be represented	*/
}

#endif

/*********************************
 * Wait for the specified number of seconds, milliseconds or microseconds,
 * respectively. Since these functions use the DOS timer, the granularity
 * depends on DOS. Some versions of DOS cannot do better than 1 second
 * granularity, a pity.
 */

#ifdef Asleep

#ifdef __OS2__
extern unsigned _far _pascal DOSSLEEP(unsigned long);

void sleep(time_t seconds)
{
    DOSSLEEP(seconds * 1000l);
}

void msleep(unsigned long milliseconds)
{
    DOSSLEEP(milliseconds);
}

void usleep(unsigned long microseconds)
{
    DOSSLEEP(microseconds / 1000l);
}

#elif _WINDOWS

#include <windows.h>

void sleep(time_t seconds)
{   time_t endtime;

    if (seconds == 0)
      {
      MSG   msg;

      MessageLoop(&msg);	/* yield to other Windows programs	*/
      }
    endtime = time(0) + seconds;
    while (time(0) < endtime)
	;
}

void msleep(unsigned long milliseconds)
{   clock_t endtime;

    endtime = clock() + milliseconds / (1000 / CLK_TCK);
    while (clock() < endtime)
	;
}

void usleep(unsigned long microseconds)
{   clock_t endtime;

    endtime = clock() + microseconds / (1000000 / CLK_TCK);
    while (clock() < endtime)
	;
}

#else

void sleep(time_t seconds)
{   time_t endtime;

    endtime = time(0) + seconds;
    while (time(0) < endtime)
	;
}

void msleep(unsigned long milliseconds)
{   clock_t endtime;

    endtime = clock() + milliseconds / (1000 / CLK_TCK);
    while (clock() < endtime)
	;
}

void usleep(unsigned long microseconds)
{   clock_t endtime;

    endtime = clock() + microseconds / (1000000 / CLK_TCK);
    while (clock() < endtime)
	;
}
#endif

#endif

/****************************************
 * Some of the wierdness in this routine is to make it reentrant.
 */

#ifdef Astrf

#define PUT(c)	{ if (nwritten >= maxsize)	\
		    goto error;			\
		  s[nwritten++] = (c);		\
		}

#define PUTA(s,l) p = (s); length = (l); goto puta
#define PUTS(s)	p = (s); goto puts
#define PUT2(v)	n = (v); goto put2
#define PUT3(v)	n = (v); goto put3
#define PUT4(v)	n = (v); goto put4

size_t strftime(char *s, size_t maxsize, const char *format,
	const struct tm *timeptr)
{   char c;
    char *p;
    int n;
    size_t length;
    char buf[19];
    size_t nwritten = 0;

    static char fulldays[7][10] =
    { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };

    static char months[12][10] =
    { "January","February","March","April","May","June","July","August",
      "September","October","November","December" };

    while ((c = *format++) != 0)
    {
	if (c == '%')
	{
	    switch (*format++)
	    {
		case 'a':		/* abbreviated weekday name	*/
		    PUTA(fulldays[timeptr->tm_wday],3);
		    break;
		case 'A':		/* full weekday name		*/
		    PUTS(fulldays[timeptr->tm_wday]);
		    break;
		case 'b':		/* abbreviated month name	*/
		    PUTA(months[timeptr->tm_mon],3);
		    break;
		case 'B':		/* full month name		*/
		    PUTS(months[timeptr->tm_mon]);
		    break;
		case 'c':		/* date and time representation	*/
		    p = "%X %x";
		    goto strf;
		case 'd':		/* day of month as 01..31	*/
		    PUT2(timeptr->tm_mday);
		    break;
		case 'H':		/* hour as 00..23		*/
		    PUT2(timeptr->tm_hour);
		    break;
		case 'I':		/* hour as 01..12		*/
		    n = timeptr->tm_hour - 1;
		    if (n < 0)
			n = 24;
		    PUT2((n % 12) + 1);
		    break;
		case 'j':		/* day of year as 001..366	*/
		    PUT3(timeptr->tm_yday + 1);
		    break;
		case 'm':		/* month as 01..12		*/
		    PUT2(timeptr->tm_mon + 1);
		    break;
		case 'M':		/* minute as 00..59		*/
		    PUT2(timeptr->tm_min);
		    break;
		case 'p':		/* AM or PM			*/
		    PUTA((timeptr->tm_hour < 12 ? "AM" : "PM"),2);
		    break;
		case 'S':		/* second as 00..59		*/
		    PUT2(timeptr->tm_sec);
		    break;
		case 'U':		/* week as 00..53 (1st Sunday is 1st day of week 1) */
		    PUT2((timeptr->tm_yday - timeptr->tm_wday + 7) / 7);
		    break;
		case 'w':		/* weekday as 0(Sunday)..6	*/
		    PUT2(timeptr->tm_wday);
		    break;
		case 'W':		/* week as 00..53 (1st Monday is 1st day of week 1) */
		    PUT2((timeptr->tm_yday - ((timeptr->tm_wday + 6) % 7) + 7) / 7);
		    break;
		case 'x':		/* appropriate date representation */
		    p = "%d-%b-%y";
		    goto strf;
		case 'X':		/* appropriate time representation */
		    p = "%H:%M:%S";
		strf:
		    if (strftime(buf,sizeof(buf),p,timeptr) == 0)
			goto error;
		    PUTS(buf);
		    break;
		case 'y':		/* year of century (00..99)	*/
		    PUT2(timeptr->tm_year % 100);
		    break;
		case 'Y':		/* year with century		*/
		    PUT4(timeptr->tm_year + 1900);
		    break;
		case 'Z':		/* time zone name or abbrev	*/
		    /* no time zone is determinable, so no chars	*/
		    break;
		case '%':
		    PUT('%');
		    break;
	    }
	}
	else
	    PUT(c);
	continue;

put2:	length = 2;
	goto putn;

put3:	length = 3;
	goto putn;

put4:	length = 4;
putn:	buf[0] = (n / 1000) + '0';
	buf[1] = ((n / 100) % 10) + '0';
	buf[2] = ((n / 10) % 10) + '0';
	buf[3] = (n % 10) + '0';
	p = buf + 4 - length;
	goto puta;

puts:	length = strlen(p);
puta:	if (nwritten + length >= maxsize)
	    goto error;
	memcpy(s + nwritten,p,length);
	nwritten += length;
    }
    if (nwritten >= maxsize)
	goto error;
    s[nwritten] = 0;
    return nwritten;		/* exclude terminating 0 from count	*/

error:
    return 0;
}

#endif /* Astrf */

/************************************
 */

#ifdef Atime3

#undef difftime

double difftime(time_t t1,time_t t2)
{
    return t1 - t2;
}

#undef gmtime

struct tm *gmtime(const time_t *timer)
{
    return NULL;	/* UTC not available	*/
}

#endif
