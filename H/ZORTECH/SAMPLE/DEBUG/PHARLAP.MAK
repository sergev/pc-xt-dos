#
#	makefile to build examples for the Phar Lap version of
#	the Zortech Debugger
#
ZTC	=	ztc -mp -g

all : exam1.exe exam2.exe exam3.exe exam4.exe exam5.exe \
      exam7.exe vectest.exe

clean :
	del *.obj
	del *.exe
	del *.map

exam5.exe : exam5.c exam5a.c exam5b.c
	$(ZTC) exam5 exam5a exam5b

vectest.exe : vectest.cpp vector.cpp
	$(ZTC) vectest vector

.c.exe :
	$(ZTC) $*

