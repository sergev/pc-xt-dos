#include <bios.h>
#include <box.hpp>
#include <stdlib.h>

char a[] = { '\0','\7','\x70' };

main()
{
    zBox b(6,6,12,30,a,1);
    b.title("Brer Rabbit");
    b.printf(
"Was found in\n\
possesion of\n\
%d briar patches",
                3);
    bioskey(0);
    return EXIT_SUCCESS;
}
