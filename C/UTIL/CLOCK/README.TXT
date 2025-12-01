Diamond Flower's MBI-MIO Card and DIO-200X Card
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
With these cards, Diamond Flower supply 2 programs, GETCLOCK.COM and SETCLOCK.COM.

GETCLOCK fetches the date/time from the card and uses it to
update the system clock in DOS. A reference to GETCLOCK is
placed in the AUTOEXEC.BAT file so that GETCLOCK is run each
time the computer is turned on.

SETCLOCK is only used when the card's date/time becomes
incorrect (or to set up the card after battery replacement). To
correct it, DOS' date and time commands are used after SETCLOCK
is run, ie.

    C:\>setclock
    C:\>date
    C:\>time

Download:
    https://www.minuszerodegrees.net/rtc/diamondflower/mbimio/GETCLOCK.COM
    https://www.minuszerodegrees.net/rtc/diamondflower/mbimio/SETCLOCK.COM

Details:
    https://www.minuszerodegrees.net/rtc/rtc.htm

Hardware:
    MM58167A chip, using I/O ports of range 2C0 - 2DF
    (source: page 2-2 of user's manual)

Sources:
    https://github.com/wilco2009/RTC_micro8088.git
