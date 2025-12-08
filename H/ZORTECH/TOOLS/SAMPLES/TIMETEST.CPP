// TIMETEST.CPP
#include <stdlib.h>
#include <time.hpp>

main()
{
//. Declare two ÝzTimeInfo objects, one, Ýti initialized from the
//. sytem clock, and the other by a date string.
    zTimeInfo ti, apr1 = "4/1/91";

//. Demonstrate the date/time formatting functions, then do some
//. date arithmetic.
    printf("Today is %s - time: %s\n",
        ti.dldate(),ti.tod());
    if (ti > apr1)
        printf("%ld days after ",
        (ti-apr1)/SECSPERDAY);
    else if (ti < apr1)
        printf("%ld days before ",
            (apr1-ti)/SECSPERDAY);
    printf("April fools day\n");
// check addition and assignment
    ti = apr1+364*SECSPERDAY;
    printf("ti becomes %s - time: %s\n",
            ti.dldate(),ti.tod());

// modify ti to November 5 - same year
    ti.set(-1,11,5);

// and the other date formats
    printf("Other date formats are:\n");
    printf("    %s\n",ti.ldate());
// then to July 4 1990 12:30
    ti.set(90,7,4,12,30);
    printf("    %s\n",ti.date());
    printf(
"20 days after this last date it will be ");

// check the copy constructor
    zTimeInfo then = ti;
    then += 20*SECSPERDAY;
    printf("%s - time: %s\n",
        then.dldate(),then.tod());
    return EXIT_SUCCESS;
}
