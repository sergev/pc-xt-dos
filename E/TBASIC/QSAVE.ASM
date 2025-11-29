              title          QSAVE
              page           60,132

dest          equ            [bp + 06h]
cols          equ            [bp + 0Ah]
rows          equ            [bp + 0Eh]
col           equ            [bp + 12h]
row           equ            [bp + 16h]

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
              les            di,rows
              mov            cx,es:[di]
              push           cx
              push           dx
              xor            bx,bx
              mov            bl,ah
              les            di,row
              mov            ax,es:[di]
              dec            ax
              mul            bx
              shl            ax,1
              les            di,col
              mov            bx,es:[di]
              dec            bx
              shl            bx,1
              add            bx,ax
              mov            si,bx
              les            di,cols
              mov            bx,es:[di]
              les            di,dest
              cld
              pop            dx
nxtrow1:      push           si
              cli
              mov            cx,bx
waitlo1:      in             al,dx
              test           al,01
              jnz            waitlo1
waithi1:      in             al,dx
              test           al,01
              jz             waithi1
              movsw
              loop           waitlo1
              sti
              pop            si
              pop            cx
              dec            cx
              jcxz           endit
              push           cx
              add            si,160
              jmp            short nxtrow1
endit:        pop            es
              pop            ds
              pop            bp
program       ends
              end
