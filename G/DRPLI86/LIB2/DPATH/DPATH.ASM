        page    60,132
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;  Copyright (C) 1984  David Micon
;
;  This code may be freely copied or modified, but not sold for
;  profit.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        title   data path
        name    dpath
 
CR      equ     0dh
 
intseg  segment at 0
        org     21h*4
int21   label   word
intseg  ends
 
code    segment para
        assume  cs:code
        org     100h
start:
        jmp     loaddpath
oldint21 dd     ?               ;old interrupt
lstptr  dw      ?               ;dir lst ptr
defdrv  db      ?               ;default drive
func    db      ?               ;interrupt function
alsave  db      ?               ;interrupt al
loadflag db     ?               ;load interrupt flag
 
        even
dpathid equ     4213h
id      dw      0
newint21 label  far
        sti
        cmp     ah,0fh          ;open?
        je      doint0f         ;yes
        cmp     ah,17h          ;rename?
        je      doint0f         ;yes
        jmp     oldint21
doint0f:
        mov     func,ah         ;save function
        mov     alsave,al       ;save al
        pushf
        call    oldint21        ;do open
        cmp     al,0            ;in current dir?
        jne     notincurrent    ;no
        iret
notincurrent:
        mov     lstptr,81h      ;initialize lstptr
        mov     ah,19h
        int     21h             ;get default drive
        mov     defdrv,al       ;save default drive
        push    dx
        push    si
        push    ds
        push    cs
        pop     ds
        mov     si,0
        mov     byte ptr [si],'\' ;put in initial char
        inc     si              ;to next char
        xor     dl,dl           ;do default drive
        mov     ah,47h
        int     21h             ;get current directory
        pop     ds
        pop     si
        pop     dx
lstloop:
        push    di
        push    ds
        push    cs
        pop     ds              ;ds=cs
        mov     di,ds:lstptr
        cmp     byte ptr [di],0 ;done?
        je      lstdone         ;yes
        push    dx
        mov     dl,defdrv       ;get default drive
        mov     ah,0eh
        int     21h             ;set default drive
        cmp     byte ptr [di+1],':' ;drive change needed?
        jne     nochgdrv        ;no
        mov     dl,[di]         ;get drive letter
        and     dl,0dfh         ;make upper case
        sub     dl,'A'          ;convert to binary
        mov     ah,0eh
        int     21h             ;change default drive
nochgdrv:
        mov     dx,di           ;point to string
        mov     ah,3bh
        int     21h             ;change default directory
        pop     dx
        pop     ds
        pop     di
        jc      nxtlst          ;br if dir does not exist
        mov     ah,func         ;set function
        mov     al,alsave       ;restore al
        pushf
        call    oldint21        ;do function
        cmp     al,0            ;function ok?
        je      leave           ;yes
nxtlst:
        push    cx
        push    di
        push    es
        push    cs
        pop     es              ;es=cs
        mov     di,lstptr       ;get list ptr
        mov     cx,80h          ;set count
        mov     al,0
        repnz   scasb           ;look for null
        mov     lstptr,di       ;set new list ptr
        pop     es
        pop     di
        pop     cx
        jmp     lstloop
lstdone:
        pop     ds
        pop     di
        mov     ah,func         ;put in func
        mov     al,0ffh
leave:
        push    ax
        push    dx
        push    ds
        push    cs
        pop     ds              ;ds=cs
        mov     dl,defdrv
        mov     ah,0eh
        int     21h             ;restore default drive
        mov     dx,0
        mov     ah,3bh
        int     21h             ;set dir
        pop     ds
        pop     dx
        pop     ax
        iret
 
loaddpath:
        call    srchdpath       ;dpath installed?
        jnc     loaded          ;yes
        mov     loadflag,1      ;set load flag
        push    cs
        pop     es
        jmp     short procargs
loaded:
        mov     loadflag,0      ;reset load flag
        mov     ax,es           ;get search seg
        xor     dx,dx           ;clear high part
        mov     cx,4
shftlp:
        shl     ax,1            ;shift low
        rcl     dx,1            ;shift high
        loop    shftlp
        add     ax,di           ;add offset
        adc     dx,0            ;add possible carry
        sub     ax,(offset id-offset start)+102h ;sub offset
        sbb     dx,0            ;sub possible borrow
        mov     cx,4
shftlp1:
        shr     dx,1            ;shift high
        rcr     ax,1            ;shift low
        loop    shftlp1
        mov     es,ax           ;point to old segment
procargs:
        push    cs
        pop     ds              ;ds=cs
        mov     si,81h          ;point to src
        mov     di,si           ;point to dst
        cld                     ;move forward
        mov     byte ptr es:[di-1],1 ;make byte before 1st arg non-zero
firstlp:
        lodsb
        cmp     al,' '          ;blank?
        je      firstlp         ;yes
        cmp     al,CR           ;CR?
        jne     argloopst       ;no
        cmp     loadflag,0      ;loaded?
        je      prtdpath        ;yes
        mov     dx,offset noloadmsg
        mov     ah,9
        int     21h             ;print msg
        int     20h
prtdpath:
        mov     dx,offset dpathmsg
        mov     ah,9
        int     21h             ;print dpath msg
        dec     si              ;move back
        push    es
        pop     ds              ;ds=es
        mov     si,81h          ;point to beginning of path names
prtloop:
        lodsb                   ;get byte
        cmp     al,0            ;null?
        je      prtpathend      ;yes
        mov     dl,al
        mov     ah,2
        int     21h             ;print char
        jmp     prtloop
prtpathend:
        mov     dl,';'
        mov     ah,2
        int     21h             ;print semicolon
        cmp     byte ptr [si],0 ;at end?
        jne     prtloop         ;no
        push    cs
        pop     ds              ;ds=cs
        mov     dx,offset crlfmsg
        mov     ah,9
        int     21h             ;print CRLF
        int     20h
argloopst:
        dec     si              ;move back
argloop:
        lodsb                   ;get arg char
        cmp     al,CR           ;CR?
        je      docr            ;yes
        cmp     al,' '          ;space?
        je      argloop         ;yes, skip
        cmp     al,';'          ;semicolon?
        jne     notsemi         ;yes
        mov     al,0            ;set null
notsemi:
        stosb                   ;put char in dst
        jmp     argloop
docr:
        mov     al,0
        cmp     al,byte ptr es:[di-1] ;already null at end?
        je      nullatend       ;yes
        stosb                   ;put in null
nullatend:
        stosb                   ;put in null
        cmp     loadflag,0      ;load?
        jne     doload          ;yes
        int     20h
doload:
        mov     dx,offset loadmsg
        mov     ah,9
        int     21h
        mov     id,dpathid
        mov     ax,intseg
        mov     ds,ax
        assume  ds:intseg
        mov     ax,int21+2      ;get int seg
        mov     word ptr oldint21+2,ax ;save it
        mov     ax,int21        ;get int offset
        mov     word ptr oldint21,ax ;save it
        mov     int21+2,cs      ;set new int seg
        mov     int21,offset newint21 ;set new int offset
        assume  ds:nothing
        mov     dx,offset loaddpath
        int     27h
 
srchdpath proc  near
        xor     ax,ax           ;start at segment 0
        cld
blkloop:
        mov     es,ax           ;set segment
        mov     cx,4000h        ;set count
        mov     di,0
        mov     ax,dpathid
loop1:
        repnz   scasw           ;look for 1st word
        jnz     nxtblk          ;no dpath found in this 32k block
        push    cx              ;save cnt
        push    di              ;save ptr
        mov     si,offset id+2  ;set code offset
        mov     cx,(offset loaddpath-offset id)-2
        repz    cmpsb           ;see if rest is here
        pop     di              ;restore ptr
        pop     cx              ;restore cnt
        jnz     loop1           ;search unsuccessful
        clc                     ;set found flag
        ret
nxtblk:
        mov     ax,es           ;get es
        add     ax,800h         ;to next block
        mov     dx,cs
        cmp     ax,dx           ;done?
        jb      blkloop         ;no
        stc                     ;set not found flag
        ret
srchdpath endp
 
loadmsg db      'Resident part of dpath loaded',13,10,'$'
noloadmsg db    'Dpath not loaded'
crlfmsg db      13,10,'$'
dpathmsg db     'DPATH=$'
code    ends
        end     start
                                                                                                                                      