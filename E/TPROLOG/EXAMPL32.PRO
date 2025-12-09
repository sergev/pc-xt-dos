               /* Program 32 */
goal
     makewindow(1,7,7,"Timer",8,10,12,60),
     time(0,0,0,0),system("dir a:"),
     time(H,M,S,Hundredths),
     write(H," hours  "),
     write(M," minutes  "),
     write(S," seconds  "),
     write(Hundredths," hundredths of a second"),nl,nl.
