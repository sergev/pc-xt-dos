              title          RECOLORS
              page           60,132

oldattr       equ            [bp + 0Ah]          ;
newattr       equ            [bp + 06h]          ;

program       segment
              assume         cs:program
              push           bp
              mov            bp,sp
              push           ds
              push           es
              mov            ah,15
              int            10h
              cmp            al,7
              jnz            color
              mov            bx,0B000h
              mov            dx,03BAh
              jmp            short ok
color:        mov            bx,0B800h
              mov            dx,03DAh
ok:           push           bx
              pop            ds
              mov            si,0001h
SetScrn:      les            di,oldattr          ; get old attribute
              mov            cx,es:[di]          ; to CX
              les            di,newattr          ; get new attribute
              mov            bx,es:[di]          ; to BX
              mov            bh,cl               ; put old in BH,new in BL
              mov            cx,07d0h            ; do whole screen
              mov            di,0002             ; use for increment
              cld                                ; clear direction flags
              cli                                ; disable interrupts
Waitlo1:      in             al,dx               ; get status
              shr            al,1                ; is it low?
              jc             WaitLo1             ; wait until it is
WaitHi1:      in             al,dx               ; get status
              shr            al,1                ; is it high?
              jnc            WaitHi1             ; wait until it is
              cmp            [si],bh
              jnz            colorok1
              mov            [si],bl             ; put the new attribute
ColorOK1:     add            si,di               ; increment screen pointer
              loop           Waitlo1             ; do it until CX=0
              sti
endit:        pop            es                  ; restore DS
              pop            ds                  ; restore ES
              pop            bp                  ; back to basic
program       ends
              end


