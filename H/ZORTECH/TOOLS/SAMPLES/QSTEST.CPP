#include <bios.h>
#include <iostream.hpp>
#include <iqueue.hpp>
#include <istack.hpp>
#include <gstack.hpp>
#include <gqueue.hpp>
#include <stdlib.h>

const int FLUSH = 1;

//. For the example we will use queues and stacks of doubles, so first it
//. is necessary to define two classes to handle doubles.
declare(zGQueue,double);
declare(zGStack,double);

void gqstest()
{

//. Then having declared the new types we can use them to initialize a
//. queue and a stack for doubles.  In this case we specify that the
//. destructor is to clean up any objects which remain in the queue or
//. stack when we are through.
    zGQueue(double) x(FLUSH);
    zGStack(double) y(FLUSH);
    int i;
    double* t;

    for (i = 0; i < 10; ++i) {
        t = new double;
        *t = i+i/100.0;
        x.queue(t);
        y.push(t);
    }
    for (i = 10; i--;)
        cout << *x.dequeue() << endl;
    for (i = 10; i--;)
        cout << *y.pop() << endl;
}

//. Now define a function to construct an integer queue and an integer
//. stack, and exersise them in the same way.
void iqstest()
{
    zIQueue x;
    zIStack y;
    int i;

    for (i = 0; i < 10; ++i) {
        x.queue(i);
        y.push(i);
    }
    while (x.any())
        cout << x.dequeue() << endl;
    while (y.any())
        cout << y.pop() << endl;
}

main()
{
    cout <<
"Test generalised queue and stack first" << endl;
    cout <<
"Queue/push 10 doubles then dequeue/pop them\n" << endl;
    gqstest();
    cout << "Press ENTER to continue";
    cout.flush();
    cin.get();
    cout <<
"\nNow the specific versions for integer" << endl;
    iqstest();
    cout << "\nThats all" << endl;
    return EXIT_SUCCESS;
}
