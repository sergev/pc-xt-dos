 		      IT LOOKS LIKE WE DID IT AGAIN !!

ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿      
³Another crack by the breakers (Breaker,Crasher,Keymaster)...         ³
³                                                                     ³
³YEAH YEAH , BUT THIS ONE WASN'T SO HARD !!!                          ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
                                                                    
Assembly language programmers - read on !

Well, anyone will agree that this game just waited for someone to crack it.
'WELLTRIS' had some codes (flags - pictures) - because it's so hard to copy
the pictures we have decided to crack this game.

If you want to see what we did , simply load a debugger and place a
breakpoint at cs:60C1 - the program compares between the string you
gave it and the right string (actually, it compares only the first four
letters).

Because the programs check this codes so many times , we have decided
that instead of disabling the check and finish , we will copy the right
string instead of what you have typed.
So now the game thinks you're right.

We changed the program there - you can see by yourself.
If someone wants to change the program with a sector editor please follow:

load WELLTRI.EXE into any sector editor -
relative sector 50 offset 0C1H:

Uncracked version :

8D5ED4         lea    bx,[bp-2C]                   
031E9410       add    bx,[1094]                    
36803F2C       cmp    ss:byte ptr [bx],2C          
74EF           je     60BD                         
8D5ED4         lea    bx,[bp-2C]                   
031E9410       add    bx,[1094]                    
368A07         mov    al,ss:[bx]                   

Cracked version:

C45EFC         les    bx,[bp-04]                   
031E9210       add    bx,[1092]                    
268A5702       mov    dl,es:[bx+02]                
8D5ED4         lea    bx,[bp-2C]                   
031E9410       add    bx,[1094]                    
368817         mov    ss:[bx],dl                   
EB1D           jmp    60F5                         

The H after many other bytes in this document, simply represents hexadecimal
values - as we guess you already know - but just in case you don't know - we
mentioned it. 

Another tip - if you have an ega type : 'WELLTRI E'
-----------------------------------------------------------------------
The BREAKERS - Breaker,Crasher,Keymaster

30/01/90


                                                                    