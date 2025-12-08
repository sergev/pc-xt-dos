#ifndef __ISTACK_HPP
#define __ISTACK_HPP
#include <tools.hpp>
#include <slist.hpp>

class zIStack : zSList {
    unsigned ic;
public:
    void push(int a) { zSList::insert((void *)
#if M_I86SM || M_I86MM
                                                a);
#else
                              ((unsigned long) a));
#endif
                                                    ++ic; }
    int pop() {  return ic? (--ic, (int) zSList::get()): 0; }
    int any() { return ic; }
    zIStack(void) { ic = 0; }
    zIStack(int n) : ((void *)
#if M_I86SM || M_I86MM
                                n)
#else
              ((unsigned long) n))
#endif
    { ic = 0; }
};
#endif
