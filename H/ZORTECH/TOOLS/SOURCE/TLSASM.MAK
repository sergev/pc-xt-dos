# MAKE File for C++ Tools Assembly Modules
# Copyright (C) Zortech Ltd. 1990
# All Rights Reserved
# Written by Steve Teale & Mike LaRosa (1/16/91)

                                                                              
MODEL=S
OS=__MSDOS__
CFLAGS=-m$(MODEL) -D$(OS)
MASM=masm -Z -MX -DI8086$(MODEL) -R -D$(OS)
INC=..\include

                                                                              
A1 = bcdlow.obj lintlow.obj memx.obj
                                                                              
.asm.obj :
    $(MASM) $*;
                                                                                                                                                         

cppt$(MODEL).lib : $(A1)
    copy bcdlow.obj ..\..\lib\bcdlow$(MODEL).obj
    copy lintlow.obj ..\..\lib\lintlow$(MODEL).obj
    copy memx.obj ..\..\lib\memx$(MODEL).obj
	del *.obj
