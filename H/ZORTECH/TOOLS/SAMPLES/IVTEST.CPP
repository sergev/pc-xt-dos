#include <stdio.h>
#include <stdlib.h>
#include <iostream.hpp>
#include <intvec.hpp>

volatile int got_ctrl_c = 0;

//. In this case it is the minimally simple handler.  We just note
//. that the keyboard interrupt ocurred.
extern "C" {
int ctrl_c_handler(struct INT_DATA *)
{
    ++got_ctrl_c;   // just note that one occurred
    return 1;       // do not chain old handler
}
}

main()
{

//. Declare a ÝzIntVector on vector 0x23, to use the handler defined
//. above, with immediate installation.
    zIntVector ctrlc(0x23,ctrl_c_handler);
    char buffer[80];
    
    cout << "Enter a string possibly including ^C: ";
    cout.flush();
    gets(buffer);
    cout << "\n" << endl;
    if (got_ctrl_c)
        cout << "There was a ^C in the text you entered" << endl;
    return EXIT_SUCCESS;
}
