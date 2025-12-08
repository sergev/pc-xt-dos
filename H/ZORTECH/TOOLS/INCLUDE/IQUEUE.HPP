#ifndef __IQUEUE_HPP
#define __IQUEUE_HPP
#include <tools.hpp>
#include <slist.hpp>

class zIQueue : zSList {
    unsigned ic;
public:
    void queue(int a) { zSList::append((void *)
#if M_I86SM || M_I86MM
                                                  a);
#else
                                ((unsigned long) a));
#endif
                                         ++ic; }
    int dequeue() {  return ic? (--ic, (int) zSList::get()): 0; }
    int any() { return ic; }
    zIQueue() { ic = 0; }
    zIQueue(int n) : ((void *)
#if M_I86SM || M_I86MM
                                n)
#else
              ((unsigned long) n))
#endif
    { ic = 0; }
};
#endif
