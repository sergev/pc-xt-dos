            B R O W S I N G   T H R O U G H   F I L E S 
                          U S I N G   V I 
 
     This  file  is  the  "vi"  Browser's  Guide.   In it, we will
describe how to move backwards and  forwards through  a file while
using "vi". 

     The  first  thing  to  keep  track  of is the position of the
CURSOR.  The cursor is  a  small  line  or  box  that  indicates a
particular position  on the  display screen.  On some systems, the
cursor blinks; on others, it does not. 

     Since you have just started using "vi", the cursor is marking
the first non-blank character on the first line of the screen (the
"B" in "BROWSING").  When you start a "vi" session by reading in a
file, the  cursor always  appears at the first non-blank character
of the file. 

     Now, press the ENTER key (labelled RETURN on some keyboards).
You will  see that  the cursor  moves down  to the first non-blank
character of the next line.   Keep pressing  ENTER.   See where it
puts the  cursor on  each line.  When you get to the bottom of the
screen, keep pressing ENTER.  You will see new lines appear one at
a time.  Keep pressing ENTER until this line is the top one on the
screen.  Then stop and read some more. 

     Press the "-" key (the minus  sign).   You will  see that the
cursor moves  up to  the first non-blank character of the previous
line.  In other words, typing  a minus  is the  opposite of typing
ENTER. 

     Keep pressing  "-" until  the cursor  gets to the top line of
the screen, then press it once  more.    You  will  see  the whole
screen move  down to  make room for an old line at the top.  Thus,
ENTER and "-" let you go backwards and forwards through  the file.
Just those two keys will let you read everything in this file.  If
you ever find that you've read everything on  the screen  and have
no other  instructions, just  keep pressing  ENTER so you can read
more of the file.   If  you want  to review  something that  is no
longer on the screen, back up with "-". 

     Of course,  using ENTER and "-" moves the cursor very slowly.
There are three good commands for  moving it  faster.   Typing "L"
(for "Low")  moves the  cursor to  the bottom  line of the screen.
Typing "H" (for "High") moves the  cursor to  the top  line of the
screen.   Typing "M" (for "Middle") moves the cursor to the middle
line of the screen.  Try all of these.  NOTE: It is important that
you use  upper case for "L", "H", and "M".  The case of letters is
important in "vi".

     Now, hold down the CTRL key and press the "D" key.   (This is
called typing  CTRL-D, sometimes shortened to "^D".)  You will see
several new lines appear at the  bottom  of  the  screen.   CTRL-D
moves Down half a screen. 

     Type CTRL-D  once more,  then type CTRL-U (hold down CTRL and
press U).  You will see that CTRL-D moves the  cursor Down  half a
screen, and  CTRL-U moves  it Up.  Using CTRL-D and CTRL-U to move
forward and backward through a file is less work than  using ENTER
and "-"; you don't have to type as many keystrokes.

     CTRL-D and  CTRL-U move  quickly, moving  half a  screen at a
time.  CTRL-F (for Forward) and CTRL-B (for  Backward) move almost
a full screen at a time.  Right now, type CTRL-F then CTRL-B.

     CTRL-F (probably) took you forward off this screen and CTRL-B
brought you back.  CTRL-F and  CTRL-B  are  good  for  moving VERY
quickly through  files.   They aren't  particularly good for going
through this "vi" guide,  however--they move  too far  in one jump
and lose what is on the screen.  Remember them for later work.

     For  practice,  use  CTRL-U  or  CTRL-B  to  go  back  to the
beginning of the file, then use CTRL-D or  CTRL-F to  come back to
this point  in the  file.  You shouldn't have trouble finding this
spot again, because it comes right before the Part II heading. 

      *****************************************************
      ******* PART II -- Horizontal Cursor Movement *******
      *****************************************************

     In the last section,  we moved  the cursor  up and  down.  In
this section,  we'll move  the cursor  back and forth on the line.
Just keep pressing ENTER or CTRL-D to see new material. 

     The cursor is likely on the first non-blank character of some
line.   On the  far right  of your keyboard, there should be a key
that shows an arrow  pointing right.   Make  sure that  the NUMBER
LOCK is  not on  (the NUM  LOCK light should be off) and press the
right arrow key.  You will  see the  cursor move  one character to
the right. 

     Pressing the  right arrow key repeatedly will keep moving the
cursor to the right, until you get to the end  of the  line.  Then
it stops because it can't go any farther. 

     Near the  right arrow  key is a key with an arrow pointing to
the left.  If you press that key, you should  see the  cursor move
to the left.  This means that you can use the left and right arrow
keys to move the cursor to any character on a line. 

     You may have noticed that there are up  and down  arrows near
the right  and left  arrows.  To see what they do, move the cursor
to the * character  on this  very line  and press  the down arrow.
You will  see the cursor moves down a line.  Pressing the up arrow
moves back to the "*". 

     The two arrows move up and down like "-" and ENTER.  However,
when you  press an  arrow, the  cursor stays in the same column on
the screen; when you press "-"  or ENTER,  the cursor  goes to the
first  non-blank  character  of  the  line.    Check  this out for
yourself. 

     Moving the cursor back and forth  on a  line can  take a long
time if  you are  using the  left and  right arrow keys.  Try this
instead.  Move the cursor to the beginning of a line and press the
"$" key.   You  will see  the cursor  jump to  the end of the same
line.  Press the "0" (zero)  key,  and  the  cursor  jumps  to the
beginning of  the line.   Thus "$" and "0" give you a quick way to
jump to the end or the beginning of a line. 

     Now move the cursor to the beginning of  this very  line, and
type "w"  (lower case).   You  will see that the cursor jumps from
the "N" in "Now" to the "m" of "move".  "w" stands  for "word" and
the  "w"  command  jumps  to  the start of the next word...more or
less. 

     Keep typing "w" and you  will  see  that  the  cursor usually
jumps from  one word to the next, but occasionally stops at things
we normally don't think of as words.  For example, it will stop at
the comma  after "line".  This is because "vi" defines a "word" as
a sequence of alphabetic/numeric characters or a sequence  of NON-
alphabetic/numeric  characters  (not  including  blanks).    As  a
result, "vi"  regards punctuation  characters as  "words".  You'll
quickly get used to this. 

     Keep on  pressing "w",  and you will see the cursor jump from
the last word on one line to the first word of the next.  Pressing
"w" goes  to the  beginning of  the next  word, even if that means
going to the next line. 

     Now press "b" (lower  case).   You will  see that  the cursor
jumps backwards  to the  beginning of  a word.   If  the cursor is
already at the beginning of a word, it will jump back to the start
of the  previous word.  However, if you use the arrow keys to move
the cursor to the middle of a word, pressing "b" goes to the start
of the word that contains the cursor.  Try it.

      *****************************************************
      *******         PART III -- Searching         *******
      *****************************************************

     You now know how to move the cursor up and down on the screen
and back and forth on the line.  In this part of  the guide, we'll
discuss another way to move through text: searching.

     When you  read in  a file, "vi" puts the cursor at the file's
first line, but often what you want to look at is in the middle of
the file.   You  can use  CTRL-D to work your way down through the
file...but this can take time.  If you know what  sort of material
you  are  looking  for,  it  is  often  faster to use the "search"
facility.  This tells  "vi" to  search through  the file  from the
current  cursor  position  to  the next occurrence of a particular
word or phrase.

     To see how it works, start by pressing CTRL-D or  ENTER until
this entire  paragraph is  on the  screen.  Move the cursor to the
beginning of THIS line and type a slash  (/).   Be careful  not to
confuse this with the backslash (\).  When you type the slash, you
will see the cursor move down to the  bottom of  the screen.   Now
type the  word "hello",  then press ENTER. You will see the cursor
jump to the beginning of the word "hello" above.  When you  type a
word or  phrase after  a slash, "vi" searches forward for the next
occurrence of that string of characters.  The cursor is  placed on
the first character of the string that is found.

     Now, move the cursor to the beginning of this line and type a
slash followed by ENTER.    You  will  see  that  "vi"  finds THIS
occurrence  of  "hello"  <---    If  you just type a slash without
anything after it, "vi" looks for the most  recent word  or phrase
you searched for.

     Now, move  the cursor  to this  line and type a question mark
(?) followed by the word "hello", then press ENTER.   You will see
that "vi" moves the cursor BACKWARDS to the previous occurrence of
"hello".  When you type a  word or  phrase after  a question mark,
"vi" searches  backwards for  that string  of characters.   As you
might guess, if you just type a question  mark followed  by ENTER,
"vi"  searches  backwards  for  the most recent thing you searched
for.  Try it.

     When you type in characters after  a slash  or question mark,
you should  make sure  you enter  them in  the correct  case.  For
example, ask "vi" to search for  "hello",  but  type  the  word in
upper case.   You  will see  that "vi" prints the message "Pattern
not found" at the bottom of  the screen.   As  it turns  out, this
file does  not contain the word "hello" in upper case, although it
has the word many times in lower case.

     Notice that the message used the word "Pattern".   In  a "vi"
command,  anything  after  a  slash  or  question mark is called a
pattern.

     In order to make  searching more  useful, "vi"  gives special
meanings  to  several  characters  when they are used in patterns.
For example, the circumflex or caret character (^)  stands for the
beginning of a line.  Move the cursor to this line and type

                 /^line

"vi" will look for the word "line" occurring at the beginning of a
line (which is this one).

     The end of a line is  represented  by  the  dollar  sign ($).
Move the cursor to this line and type

                 /in$

You will  see that  "vi" searches  forward for a line that ends in
the word "in" (the previous line).

     Inside patterns, the dot (.) stands  for any  character.  For
example, move the cursor to the beginning of this line and type

                 /t.e

You will  see that  the cursor  moves to the word "the".  Type "/"
over and over, and you will see the cursor keep jumping forward to
any sequence  of three  letters that  starts with  "t" and ends in
"e".  Were you surprised that the cursor jumped into the middle of
the word  "letters"?  "vi" finds character strings, even when they
are in the middle of larger words.

     Whenever you enter a pattern, REMEMBER that dot has a special
meaning.   It may  look like  the  end of a sentence, but don't be
fooled.  For example,  move the  cursor to  the beginning  of this
paragraph and type

                 /end of a sentence.

The cursor  moves to  the string "end of a sentence,".  The dot in
the pattern matches the  comma in  the actual  string.   This is a
mistake that beginners frequently make.

     Inside patterns,  a dot followed by a star (*) stands for any
sequence of zero or more characters.  For example, move the cursor
to this line and type

                 /^Y.*s$

You will  find the next line that begins with the letter "Y", ends
with the letter "s" and has any number of characters in between.

     "vi"  gives  special  meanings  to  several  other characters
inside patterns.   For complete details, see Part 5 of the Toolkit
Reference Manual, describing  "Regular  Expressions".    A regular
expression is  the UNIX  name for a pattern; in this guide, we use
the word "pattern" because it is more descriptive.

     What happens if you want to search for a character that has a
special meaning  in patterns?   For  example, suppose  you want to
search for the string "$1.00" somewhere  in a  file.   If you just
say

                 /$1.00

"vi" will think you are looking for "1.00" appearing AFTER the end
of the line (which is impossible, since nothing ever appears after
the end  of a  line).  In such cases, put a backslash (\) in front
of any characters with special meanings, as in

                 /\$1\.00

Notice that we had to put a backslash in front of the  dot as well
as the dollar sign; both have a special meaning in patterns.

     By default,  all searches in "vi" wrap around from the bottom
of the file to the top.  For example, consider a string consisting
of two  per cent  signs (%%).  This line is the only place in this
file where that string occurs.  Now move  the cursor  to THIS line
and search FORWARD for the two at-signs.  The search will start by
going down to the bottom of the file.   Since  "vi" will  not find
the string  by then,  "vi" will go back to the top of the file and
keep searching, eventually finding the string on the above line.

     Similarly, if  you  use  question  marks  to  search backward
through a  file, the  search will  wrap around from the top of the
file to the bottom, if necessary.

      *****************************************************
      *******        PART IV -- Getting Help        *******
      *****************************************************

     Here's a simple trick that will let you get help with "vi" in
the middle  of a  "vi" session.  The Toolkit's "help" command will
supply a  brief description  of every  "vi" command.   The command
displays quite  a lot  of material,  so you  should be prepared to
press CTRL-S when you execute the "help" command.  Pressing CTRL-S
puts a  temporary stop to the material "help" displays, giving you
a chance to read what is on your screen.  When you are ready to go
on, press  CTRL-Q and  be prepared to press CTRL-S again.  In this
way, you can look through the "help" material a bit at a time.

     To issue the "help" command, type

                 :!help vi

You will notice that when you type the  colon, the  cursor goes to
the bottom  of the  screen.   When you have typed this line, press
ENTER.  Remember to be ready  to enter  CTRL-S.   Also remember to
enter CTRL-Q  when you  want to  see more  output.  When "help" is
finished displaying the help material, it will return to your "vi"
session.

      *****************************************************
      *******      PART  V -- Moving to a Line      *******
      *****************************************************

     This last  part we had to save until the end because it tells
you a quick way to go to the end of a file.  Just  type "G" (upper
case) and  the cursor  will go  to the  last line.   If you type a
number before the "G", the cursor  will move  to that  line in the
file.   For example, "1G" goes to the first line of the file.  Try
going to the first line of the file, then coming back here  to the
end.

     Remember, to quit "vi" now, type ":q!" and press ENTER.

