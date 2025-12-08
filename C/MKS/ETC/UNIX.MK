# Default rules (merged DOS/UNIX/XENIX version)

HAVERCS=yes		# If you do not have MKS RCS, set to no

# Allow redefinition on command line of macros defined here,
# without warning messages
__SILENT:=$(.SILENT)
.SILENT:=yes

# suffix definitions
.IF $(OS)==DOS
E:=.exe
O:=.obj
S:=.asm
A:=.lib
P:=.pas			# Pascal
F:=.for			# Fortran
V:=			# no RCS suffix
.ELSE
E:=
O:=.o
S:=.s
A:=.a
P:=.p
F:=.f
V:=,v
.END

# Other macros
.IMPORT: ROOTDIR
TMPDIR:=$(ROOTDIR)/tmp
SHELL:=$(NULL)
.IMPORT .IGNORE: TMPDIR SHELL COMSPEC

# If SHELL not defined, use COMSPEC, assumed to be command.com.
# If SHELL is defined, it is assumed to be the MKS Toolkit Korn Shell

.IF $(SHELL)==$(NULL)
.IF $(COMSPEC)==$(NULL)
    SHELL:=$(ROOTDIR)/bin/sh$E
.ELSE
    SHELL:=$(COMSPEC)
.END
.END
GROUPSHELL := $(SHELL)

.IF $(SHELL)==$(COMSPEC)
    DIRSEPSTR := \/:
    SHELLMETAS := *?"<>|
    SHELLFLAGS := $(SWITCHAR)c
    GROUPFLAGS := $(SHELLFLAGS)
    GROUPSUFFIX := .bat
.ELSE
    SHELLMETAS := *?"<>|()&][$$\#`'
    SHELLFLAGS := -c
    GROUPSUFFIX := .ksh
    GROUPFLAGS := $(NULL)
.END

# Command names
CC	:= anycc		# Your C/C++ compiler
LD	:= anylink		# Your linker
AS	:= anyasm		# Your assembler

MAKE	= $(MAKECMD) $(MFLAGS)
CO	:= co			# MKS RCS Check out
AR	:= ar			# MKS AR librarian, provided with MKS Toolkit
RM	:= rm			# UNIX-style rm, provided with MKS Toolkit
YACC	:= yacc			# MKS LEX&YACC
LEX	:= lex
YACCR	:= yacc -r		# System V YACC for Ratfor/Efl
YACCE	:= yacc -e

PC	:= anypc		# Your Pascal compiler
FC	:= anyf77		# Your Fortran compiler
RC	:= f77			# Ratfor

.IF $(OS)==DOS
LEXYY	:= lex_yy
YTAB	:= ytab
.ELSE
LEXYY	:= lex.yy
YTAB	:= y.tab
RANLIB	:= ranlib
.END

# Command flags and default args
ARFLAGS=ruv
CFLAGS=
ASFLAGS=
COFLAGS=-q			# check-out
LDFLAGS=
LDLIBS=				# additional libraries
LFLAGS=				# for LEX
YFLAGS=				# for YACC
RFLAGS=				# for Ratfor
RMFLAGS=			# for RM
FFLAGS=				# for FORTRAN
EFLAGS=				# for EFL
PFLAGS=				# for Pascal

# Implicit generation rules
# We don't provide .f, .yr, or .ye rules here.  They may be added.

%$O: %.c;	$(CC) -c $(CFLAGS) $^
%$O: %.cpp;	$(CC) -c $(CFLAGS) $^
%$O: %$S;	$(AS) $(ASFLAGS) -o $@ $^
%$O: %$P;	$(PC) -c $(PFLAGS) $^

%$E: %$O;	$(LD) $(LDFLAGS) -o $@ $& $(LDLIBS)
%$E:	;	$(LD) $(LDFLAGS) -o $@ $& $(LDLIBS)

.IF	$(CC)==$(LD)
%$E: %.l;	$(LEX) $(LFLAGS) $^
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(LEXYY).c $(LDLIBS)
		$(RM) $(LEXYY).c
%$E: %.y;	$(YACC) $(YFLAGS) $^
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(YTAB).c $(LDLIBS)
		$(RM) $(YTAB).c
%$E: %.c;	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
%$E: %$S;	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
.END

%$O: %$F; $(FC) $(RFLAGS) $(EFLAGS) $(FFLAGS) -c $^
%$O: %.r; $(FC) $(RFLAGS) $(EFLAGS) $(FFLAGS) -c $^
%$O: %.e; $(FC) $(RFLAGS) $(EFLAGS) $(FFLAGS) -c $^

%.c: %.l;	$(LEX) $(LFLAGS) -o $@ $^
%$O: %.l;	$(LEX) $(LFLAGS) -o $*.c $^
		$(CC) $(CFLAGS) -c $*.c

.IF	$(OS)==DOS
%.c: %.y;	$(YACC) $(YFLAGS) -o $@ $^
%$O: %.y;	$(YACC) $(YFLAGS) -o $*.c $^
		$(CC) $(CFLAGS) -c $*.c
.ELSE
%.c: %.y;	$(YACC) $(YFLAGS) $^
		mv $(YTAB).c $@
.END

# Intermediate target file removal
.REMOVE:;	$(RM) $(RMFLAGS) $?

# RCS support
.IF $(HAVERCS)==yes
.IF $V!=$(NULL)
% .PRECIOUS: %$V;		-$(CO) $(COFLAGS) $^
%.c .PRECIOUS: %.c$V;		-$(CO) $(COFLAGS) $^
%.h .PRECIOUS: %.h$V;		-$(CO) $(COFLAGS) $^
%.cpp .PRECIOUS: %.cpp$V;	-$(CO) $(COFLAGS) $^
%.hpp .PRECIOUS: %.hpp$V;	-$(CO) $(COFLAGS) $^
%.l .PRECIOUS: %.l$V;		-$(CO) $(COFLAGS) $^
%.y .PRECIOUS: %.y$V;		-$(CO) $(COFLAGS) $^
%$P .PRECIOUS: %$P$V;		-$(CO) $(COFLAGS) $^
%$S .PRECIOUS: %$S$V;		-$(CO) $(COFLAGS) $^
%$F .PRECIOUS: %$F$V;		-$(CO) $(COFLAGS) $^
.END
% .PRECIOUS: RCS/%$V;		-$(CO) $(COFLAGS) $^
%.c .PRECIOUS: RCS/%.c$V;	-$(CO) $(COFLAGS) $^
%.h .PRECIOUS: RCS/%.h$V;	-$(CO) $(COFLAGS) $^
%.cpp .PRECIOUS: RCS/%.cpp$V;	-$(CO) $(COFLAGS) $^
%.hpp .PRECIOUS: RCS/%.hpp$V;	-$(CO) $(COFLAGS) $^
%.l .PRECIOUS: RCS/%.l$V;	-$(CO) $(COFLAGS) $^
%.y .PRECIOUS: RCS/%.y$V;	-$(CO) $(COFLAGS) $^
%.p .PRECIOUS: RCS/%.p$V;	-$(CO) $(COFLAGS) $^
%$S .PRECIOUS: RCS/%$S$V;	-$(CO) $(COFLAGS) $^
%$F .PRECIOUS: RCS/%$F$V;	-$(CO) $(COFLAGS) $^
.END

# Archive support

LIBSUFFIX :=	$A
%$(LIBSUFFIX) .PRECIOUS .LIBRARY:;	$(AR) $(ARFLAGS) $@ $?
.IF $(RANLIB)!=$(NULL)
		-$(RANLIB) $@
.END

# Turn on warnings
.SILENT:=$(__SILENT)

# Include local startup.mk file, if present
.INCLUDE .IGNORE:	startup.mk
.SOURCE: .
