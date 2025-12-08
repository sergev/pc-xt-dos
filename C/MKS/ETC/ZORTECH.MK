# MKS MAKE default rules for Zortech C++ 3.0 compiler on DOS

HAVERCS=yes		# If you do not have MKS RCS, set to no

# Allow redefinition on command line of macros defined here,
# without warning messages
__SILENT:=$(.SILENT)
.SILENT:=yes

# suffix definitions
E:=.exe
O:=.obj
S:=.asm
A:=.lib
P:=.pas			# Pascal
F:=.for			# Fortran

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
CC	:= ztc			# Zortech C++
LD	:= ztc			# Interface to BLINK
AS	:= masm

MAKE	= $(MAKECMD) $(MFLAGS)
CO	:= co			# MKS RCS Check out
AR	:= ar			# MKS AR librarian, provided with MKS Toolkit
RM	:= rm			# UNIX-style rm, provided with MKS Toolkit
YACC	:= yacc			# MKS LEX&YACC
LEX	:= lex

PC	:= anypc		# Your Pascal compiler
FC	:= anyf77		# Your Fortran compiler

LEXYY	:= lex_yy		# MKS LEX output
YTAB	:= ytab			# MKS YACC output

# Command flags and default args
ARFLAGS=ruv
CFLAGS=
ASFLAGS=
COFLAGS=-q			# check-out
LDFLAGS=$(CFLAGS)		# same as CFLAGS with ztc interface to BLINK
LDLIBS=				# additional libraries
LFLAGS=				# for LEX
YFLAGS=				# for YACC
RMFLAGS=			# for RM
FFLAGS=				# for FORTRAN
PFLAGS=				# for Pascal

# Implicit generation rules
# We don't provide .f rules here.  They may be added.

%$O: %.c;	$(CC) -c $(CFLAGS) $^
%$O: %.cpp;	$(CC) -c $(CFLAGS) $^
%$O: %$S;	$(AS) $(ASFLAGS) $^
%$O: %$P;	$(PC) -c $(PFLAGS) $^

# Use tcc as interface to TLINK
%$E: %$O;	$(LD) $(LDFLAGS) -o$@ $& $(LDLIBS)
%$E:	;	$(LD) $(LDFLAGS) -o$@ $& $(LDLIBS)

%.c: %.y;	$(YACC) $(YFLAGS) -o $@ $^
%.cpp: %.y;	$(YACC) -LC $(YFLAGS) -o $@ $^
%$P: %.y;	$(YACC) -LP $(YFLAGS) -o $@ $^

%.c: %.l;	$(LEX) $(LFLAGS) -o $@ $^
%.cpp: %.l;	$(LEX) -LC $(LFLAGS) -o $@ $^
%$P: %.l;	$(LEX) -LP $(LFLAGS) -o $@ $^

# Intermediate target file removal
.REMOVE:;	$(RM) $(RMFLAGS) $?

# RCS support
.IF $(HAVERCS)==yes
% .PRECIOUS: RCS/%;	-$(CO) $(COFLAGS) $^
%.c .PRECIOUS: RCS/%.c;	-$(CO) $(COFLAGS) $^
%.h .PRECIOUS: RCS/%.h;	-$(CO) $(COFLAGS) $^
%.cpp .PRECIOUS: RCS/%.cpp;	-$(CO) $(COFLAGS) $^
%.hpp .PRECIOUS: RCS/%.hpp;	-$(CO) $(COFLAGS) $^
%.l .PRECIOUS: RCS/%.l;	-$(CO) $(COFLAGS) $^
%.y .PRECIOUS: RCS/%.y;	-$(CO) $(COFLAGS) $^
%$P .PRECIOUS: RCS/%$P;	-$(CO) $(COFLAGS) $^
%$S .PRECIOUS: RCS/%$S;	-$(CO) $(COFLAGS) $^
%$F .PRECIOUS: RCS/%$F;	-$(CO) $(COFLAGS) $^
.END

# Archive support

LIBSUFFIX :=	$A
%$(LIBSUFFIX) .PRECIOUS .LIBRARY:;	$(AR) $(ARFLAGS) $@ $?

# Turn on warnings
.SILENT:=$(__SILENT)

# Include local startup.mk file, if present
.INCLUDE .IGNORE:	startup.mk
