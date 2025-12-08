# factrl take a single positive integral argument
define(`factrl', `ifelse($1,0, 1, `eval($1*factrl(eval($1-1)))')')dnl
