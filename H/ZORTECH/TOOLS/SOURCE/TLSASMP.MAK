# MAKE File for C++ Tools Assembly Modules
# Copyright (C) Zortech Ltd. 1990
# All Rights Reserved
# Written by Steve Teale & Mike LaRosa (1/16/91)
# Modified by DJB 26/6/91

#MODEL can be X or P
MODEL=X
OS=__MSDOS__
CFLAGS=-m$(MODEL) -D$(OS)
MASM=ztc -m$(MODEL) -D$(OS) -c
                                                                              
A1 = bcdlowp.obj getcp.obj lintlowp.obj memxp.obj peekp.obj peekpoke.obj
                                                                              
.asm.obj :
    $(MASM) $*

cppt$(MODEL).lib : $(A1)
    copy bcdlowp.obj ..\..\lib\bcdlowp$(MODEL).obj
    copy getcp.obj ..\..\lib\getcp$(MODEL).obj
    copy lintlowp.obj ..\..\lib\lintlow$(MODEL).obj
    copy memxp.obj ..\..\lib\memxp$(MODEL).obj
    copy peekp.obj ..\..\lib\peekp$(MODEL).obj
    copy peekpoke.obj ..\..\lib\peekpok$(MODEL).obj
    del *.obj
