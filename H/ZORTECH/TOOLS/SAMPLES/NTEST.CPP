#include <stdlib.h>
#include <bios.h>
#include <notice.hpp>

const char alist[] = { '\1', '\x30', '\x7c' };

main()
{
    zNotice n(6,6,alist,
"This is a notice\n\
click the mouse or\n\
press a key when you\n\
have read it!");
    n.wait();
    return EXIT_SUCCESS;
}
