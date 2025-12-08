#ifndef __TIME_HPP
#define __TIME_HPP
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <tools.hpp>

#define SECSPERDAY (60L*60*24)
extern char *wdays[];
extern char *months[];

class zTimeInfo {
public:

    zTimeInfo(void);
                    // set all from system
    zTimeInfo(const zTimeInfo&);
                    // copy constructor
    zTimeInfo(int, int, int = 0);
                    // set time - date from system
    zTimeInfo(char *, int=0, int=0, int=0);
                    // set all from arguments
    zTimeInfo &operator=(const zTimeInfo&);

// A set of access function return the values of the various members
// of the underlying struct tm.
    int sec() const { return ts.tm_sec; }
    int min() const { return ts.tm_min; }
    int hour() const {return ts.tm_hour; }
    int day() const { return ts.tm_mday; }
    int mon() const { return ts.tm_mon; }
    int year() const { return ts.tm_year; }
    int yday() const { return ts.tm_yday; }
    int dayw() const { return ts.tm_wday; }
    unsigned dayno() const
        { return last/SECSPERDAY; }

// The next group of functions provide character string representations
// of various aspects of the time and date.  The defaulted argument to
// tod determines whether seconds are to be displayed as part of
// the time of day.
    char *tod(int = 0);
    char *wday();
    char *dldate(); // like Tuesday 9 April 1990
    char *ldate();  // 9 April 1990
    char *date();   // 09/04/90

// A set function allows for an existing zTimeInfo object to be
// completely reset.  The arguments are year, month, day, hour,
// minute and second.
    time_t set(int, int , int , int = 0, int = 0, int = 0);

// Date and time arithmetic are facilitated by definitions of the
// +, -, += and -= operations.  The latter two affect the object for
// which they are called.
    zTimeInfo operator+(long);
    zTimeInfo operator-(long);
    long operator-(const zTimeInfo &a)
        { return last - a.last; }
    zTimeInfo &operator+=(long);
    zTimeInfo &operator-=(long);

// The boolean operators are also implemented.
    int operator>(const zTimeInfo &a)
        { return last > a.last; }
    int operator<(const zTimeInfo &a)
        { return last < a.last; }
    int operator>=(const zTimeInfo &a)
        { return last >= a.last; }
    int operator<=(const zTimeInfo &a)
        { return last <= a.last; }
    int operator==(const zTimeInfo &a)
        { return last == a.last; }
    int operator!=(const zTimeInfo &a)
        { return last != a.last; }

// The now function fixes up an existing zTimeInfo object from
// the system clock.
    time_t now();

    int error() { int e = err; err = 0; return e; }
    ~zTimeInfo() { delete dbuf; delete tbuf; }

// Set the static class variable zTimeInfo::notUS to something other
// than zero for European date styles.
    static int notUS;
private:
    struct tm ts;   // as defined in TIME.H
    time_t last;
    char *dbuf;
    char *tbuf;
    INT16 err;
};
#endif
