                title   QPRINTCS
                page    60,132

Attr            equ     [bp + 06h]      ; ATTR%
StrDat          equ     [bp + 0Ah]      ;
ColR            equ     [bp + 0Eh]      ;
ColL            equ     [bp + 12h]      ;
Row             equ     [bp + 16h]      ;

program       segment
              assume         cs:program
              push           bp
              mov            bp,sp
              push           ds
              push           es
              les            di,StrDat
              mov            cx,es:[di]
              and            cx,7FFFh
              jcxz           endit
              push           cx
              mov            dx,ds:[0]
              push           dx
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
              pop            es
              push           dx
              xor            bx,bx
              mov            bl,ah
              lds            si,row
              mov            ax,ds:[si]
              dec            ax
              mul            bx
              shl            ax,1
              lds            si,colL
              mov            bl,ds:[si]
              lds            si,colR
              mov            bh,ds:[si]
              add            bl,bh
              xor            bh,bh
              shr            bx,1
              shr            cx,1
              sub            bx,cx
              dec            bx
              shl            bx,1
              add            bx,ax
              mov            di,bx
              lds            si,attr
              mov            bx,ds:[si]
              lds            si,StrDat
              mov            si,ds:[si+2]
              pop            dx
              pop            ds
              pop            cx
              cld
              cli
waitlo1:      in             al,dx
              test           al,01
              jnz            waitlo1
waithi1:      in             al,dx
              test           al,01
              jz             waithi1
              movsb
              mov            es:[di],bl
              inc            di
              loop           waitlo1
              sti
endit:        pop            es
              pop            ds
              pop            bp
program       ends
              end
