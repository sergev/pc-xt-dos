              title          QFILLS
              page           60,132

attr          equ            [bp + 06h]
char          equ            [bp + 0Ah]
cols          equ            [bp + 0Eh]
rows          equ            [bp + 12h]
col           equ            [bp + 16h]
row           equ            [bp + 1Ah]

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
              pop            es
              lds            si,rows
              mov            cx,ds:[si]
              push           cx
              push           dx
              xor            bx,bx
              mov            bl,ah
              lds            si,row
              mov            ax,ds:[si]
              dec            ax
              mul            bx
              shl            ax,1
              lds            si,col
              mov            bx,ds:[si]
              dec            bx
              shl            bx,1
              add            bx,ax
              mov            di,bx
              lds            si,char
              mov            ax,ds:[si]
              mov            bl,al
              lds            si,attr
              mov            ax,ds:[si]
              mov            ah,al
              cld
              lds            si,cols
              mov            si,ds:[si]
              pop            dx
nxtrow1:      push           di
              mov            cx,si
              cli
waitlo1:      in             al,dx
              test           al,01
              jnz            waitlo1
waithi1:      in             al,dx
              test           al,01
              jz             waithi1
              mov            al,bl
              stosw
              loop           waitlo1
              sti
              pop            di
              pop            cx
              dec            cx
              jcxz           endit
              push           cx
              add            di,160
              jmp            short nxtrow1
endit:        pop            es
              pop            ds
              pop            bp
program       ends
              end
