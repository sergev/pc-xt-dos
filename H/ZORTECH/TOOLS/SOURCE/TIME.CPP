#include <time.hpp>

char *wdays[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

char *months[] = {
    "January",
    "Febuary",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

zTimeInfo::zTimeInfo()
{
    dbuf = new char[28];
    if (!dbuf)
        goto allerr1;
    tbuf = new char[9];
    if (!tbuf) {
allerr1:
        err = ENOMEM;
        return;
    }
    now();
    err = 0;
}

zTimeInfo::zTimeInfo(const zTimeInfo& other)
{
    dbuf = new char[28];
    if (!dbuf)
        goto allerr2;
    tbuf = new char[9];
    if (!tbuf) {
allerr2:
        err = ENOMEM;
        return;
    }
    ts = other.ts;
    last = other.last;
    memmove(dbuf,other.dbuf,28);
    memmove(tbuf,other.tbuf,9);
}

zTimeInfo::zTimeInfo(int h, int m, int s)
{
    dbuf = new char[28];
    if (!dbuf)
        goto allerr3;
    tbuf = new char[9];
    if (!tbuf) {
allerr3:
        err = ENOMEM;
        return;
    }
    now();
    ts.tm_sec = s;
    ts.tm_min = m;
    ts.tm_hour = h;
}

zTimeInfo::zTimeInfo(char *ds, int h, int m, int s)
{
    char any;
    dbuf = new char[28];
    if (!dbuf)
        goto allerr4;
    tbuf = new char[9];
    if (!tbuf) {
allerr4:
        err = ENOMEM;
        return;
    }
    if (notUS)
        sscanf(ds,"%d%c%d%c%d",&ts.tm_mday,&any,&ts.tm_mon,&any,&ts.tm_year);
    else 
        sscanf(ds,"%d%c%d%c%d",&ts.tm_mon,&any,&ts.tm_mday,&any,&ts.tm_year);
    --ts.tm_mon;
    ts.tm_sec = s;
    ts.tm_min = m;
    ts.tm_hour = h;
    last = mktime(&ts);
    ts = *localtime(&last);
}

char *zTimeInfo::tod(int show_secs) {
    sprintf(tbuf,show_secs? "%02d:%02d:%02d": "%02d:%02d",
                                ts.tm_hour,ts.tm_min,ts.tm_sec);
    return tbuf;
}

char *zTimeInfo::wday() { return wdays[ts.tm_wday]; }

char *zTimeInfo::dldate() {
    sprintf(dbuf,"%s %d %s %d",
                wday(),ts.tm_mday, months[ts.tm_mon],ts.tm_year+1900);
    return dbuf;
}

char *zTimeInfo::ldate() {
    sprintf(dbuf,"%s %d %d",months[ts.tm_mon],ts.tm_mday,ts.tm_year+1900);
    return dbuf;
}

char *zTimeInfo::date() {
    if (notUS)
        sprintf(dbuf,"%2d/%02d/%02d",ts.tm_mday,ts.tm_mon+1,ts.tm_year);
    else
        sprintf(dbuf,"%2d/%02d/%02d",ts.tm_mon+1,ts.tm_mday,ts.tm_year);
    return dbuf;
}

time_t zTimeInfo::set(int y, int m, int d, int h, int mt, int s)
{
    if (y > -1) ts.tm_year = y;
    if (m > -1) ts.tm_mon = m-1;
    if (d > -1) ts.tm_mday = d;
    if (h > -1) ts.tm_hour = h;
    if (mt > -1) ts.tm_min = mt;
    if (s > -1) ts.tm_sec = s;
    last = mktime(&ts);
    ts = *localtime(&last);  // update other fields
    return last;
}

zTimeInfo zTimeInfo::operator+(long secs)
{
    zTimeInfo t = *this;
    t.last += secs;
    t.ts = *localtime(&t.last);
    return t;
}

zTimeInfo zTimeInfo::operator-(long secs)
{
    zTimeInfo t = *this;
    t.last -= secs;
    t.ts = *localtime(&t.last);
    return t;
}

zTimeInfo &zTimeInfo::operator+=(long secs)
{
    last += secs;
    ts = *localtime(&last);
    return *this;
}

zTimeInfo &zTimeInfo::operator-=(long secs)
{
    last -= secs;
    ts = *localtime(&last);
    return *this;
}

zTimeInfo& zTimeInfo::operator=(const zTimeInfo& other)
{
    ts = other.ts;
    last = other.last;
    memmove(dbuf,other.dbuf,28);
    memmove(tbuf,other.tbuf,9);
    return *this;
}

time_t zTimeInfo::now()
{
    time(&last);
    ts = *localtime(&last);
    return last;
}
