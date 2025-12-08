# MKS MAKE default rules for WATCOM C/386 9.0 compiler on DOS.
#
# If you are using an older version of WATCOM C without WCL,
# you will have to change the linking rule below to use WLINK directly.
# The msc.mk rules file uses LINK directly; see it for a working example.

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
CC	:= wcl386		# WATCOM C
LD	:= wcl386		# WLINK
AS	:= masm			# Microsoft MASM

MAKE	= $(MAKECMD) $(MFLAGS)
CO	:= co			# MKS RCS Check out
AR	:= wlib			# use Watcom C/386 librarian
RM	:= rm			# UNIX-style rm, provided with MKS Toolkit
YACC	:= yacc			# MKS LEX&YACC
LEX	:= lex

PC	:= anypc		# Your Pascal compiler
FC	:= anyf77		# Your Fortran compiler

LEXYY	:= lex_yy		# MKS LEX output
YTAB	:= ytab			# MKS YACC output

# Command flags and default args
ARFLAGS=-c			# flags for Watcom librarian
.IF $(OS) == dos
    CFLAGS=-p			# use protected mode Watcom C compiler
.ELSE
    CFLAGS=
.END
COFLAGS=-q			# check-out
LDFLAGS=$(CFLAGS)		# same as CFLAGS with wcl interface to WLINK
LDLIBS=				# additional libraries
LFLAGS=				# for LEX
YFLAGS=				# for YACC
RMFLAGS=			# for RM
FFLAGS=				# for FORTRAN
PFLAGS=				# for Pascal

# Implicit generation rules
# We don't provide .f rules here.  They may be added.

%$O: %.c;	$(CC) $^ $(CFLAGS) -c
%$O: %$S;	$(AS) $(ASFLAGS) $^
%$O: %$P;	$(PC) -c $(PFLAGS) $^

# Use WCL as interface to WLINK
%$E: %$O;	$(LD) $(LDFLAGS) -fe=$@ $& $(LDLIBS)
%$E:	;	$(LD) $(LDFLAGS) -fe=$@ $& $(LDLIBS)

%.c: %.y;	$(YACC) $(YFLAGS) -o $@ $^

%.c: %.l;	$(LEX) $(LFLAGS) -o $@ $^

# Intermediate target file removal
.REMOVE:;	$(RM) $(RMFLAGS) $?

# RCS support
.IF $(HAVERCS)==yes
% .PRECIOUS: RCS/%;	-$(CO) $(COFLAGS) $^
%.c .PRECIOUS: RCS/%.c;	-$(CO) $(COFLAGS) $^
%.h .PRECIOUS: RCS/%.h;	-$(CO) $(COFLAGS) $^
%.l .PRECIOUS: RCS/%.l;	-$(CO) $(COFLAGS) $^
%.y .PRECIOUS: RCS/%.y;	-$(CO) $(COFLAGS) $^
%$P .PRECIOUS: RCS/%$P;	-$(CO) $(COFLAGS) $^
%$S .PRECIOUS: RCS/%$S;	-$(CO) $(COFLAGS) $^
%$F .PRECIOUS: RCS/%$F;	-$(CO) $(COFLAGS) $^
.END

# Archive support

DIRSEPSTR := \/:
LIBSUFFIX :=	$A
%$(LIBSUFFIX) .PRECIOUS .LIBRARY:
		$(AR) $(ARFLAGS) $@ @<+ -+$(?:t"-+\n") +>

# Turn on warnings
.SILENT:=$(__SILENT)

# Include local startup.mk file, if present
.INCLUDE .IGNORE:	startup.mk
