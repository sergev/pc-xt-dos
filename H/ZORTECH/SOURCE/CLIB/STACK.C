/*_ stack.c	06-Jan-85	Walter Bright	*/
/* Modified by Joe Huffman September 12, 1990
*/

/* default stack size	*/
#if DOS386
int _stack = 0x8000;
#elif __OS2__
int _stack = 0x4000;
#else
int _stack = 0x2000;
#endif

