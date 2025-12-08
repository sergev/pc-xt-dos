                    E D I T I N G   F I L E S
                         U S I N G   V I

     In  this  file,  we  discuss  commands  for entering text and
editing files.  If you  have  not  worked  through  the "browse.v"
file, you  should read that before reading this.  Also, you should
be working with a  COPY of  the "edit.v"  file, not  the original.
The work  you do while reading this file will change the file, and
you don't want to overwrite the original.  The introduction to the
"vi" guide  (on paper with the other tutorial guides) tells how to
make copies of your files.

     The "browse.v" file explained  how  to  read  through  a file
using "vi".   If  we don't  give any instructions to the contrary,
just read forward through this file by pressing ENTER or CTRL-D.

      *****************************************************
      *******        PART I -- Entering Text        *******
      *****************************************************

     The most commonly  used  command  for  entering  text  is "a"
(standing for  "append" or  "add").   The steps  for using "a" are
simple.

(1)  Determine where you want to add text and  move the  cursor to
     the character immediately before that point.

(2)  Type the  letter "a".   This  switches you  to "Insert Mode",
     where anything you type is added to the  text showing  on the
     screen.

(3)  Type in your text (as many lines as you want).

(4)  Press the  ESC key.   This  tells "vi"  that you are finished
     entering text at this  location.   In other  words, you leave
     Insert Mode.   Anything that you type now will be regarded as
     a command.

To see how this works, move around the cursor until all four steps
above are  on the screen, then move the cursor to the beginning of
the next line.  Type "a", then "hello", then press ESC.

     You should have seen the word "hello" appear on the screen as
you typed.   You  did not  see the  "a" for "append" -- that was a
command to "vi", not  text to  display on  the screen.   You might
also have  noticed that the cursor changed from a line to a block;
"vi" doesn't do this on all systems, but it does on some machines.
For people  who have  such machines,  the block indicates when you
are in Insert Mode.

     The process of creating  many lines  of text  is no different
from the  way you  entered "hello".   Type an "a", type your text,
then press ESC.  For example, try entering the first  few lines of
"Mary Had A Little Lamb" immediately after this line.

     Did you have any trouble?  Perhaps you made a typing mistake.
If you  make  mistakes  while  typing  in  text,  they're  easy to
correct.   Press the  backspace key  until you have backed up over
the error.  Once you have  done this,  you can  type over  the old
characters to  correct them.  Try it.  Move the cursor to the next
blank line, then type an "a" to enter Insert Mode.   Type "hello",
backspace back to the "h", and type "hi!".  Then press ESC.

     Notice  that  before  you  pressed  ESC, the line looked like
"hi!lo"...the old letters  from  the  end  of  "hello"  were still
visible.   After you  pressed ESC,  the "lo" disappeared.  Backing
over characters  with  backspace  removes  the  text,  even  if it
doesn't erase  the characters from the screen.  The characters are
only erased when you type over them or when you press ESC.

     If you're like many  writers, you  may change  your mind part
way through a line and want to get rid of the whole thing.  If you
are in Insert Mode and you want to  get rid  of everything  on the
line you  have just  typed in, enter an at-sign "@".  You will see
the cursor jump back to the  first character  you inserted  on the
current line.   You  can then  type over the old line.  (Note that
this does not work outside of Insert Mode.)

     So far, we've just been talking about adding new lines.   You
can  add  text  to  an  existing  line too.  For example, move the
cursor to the end of the next line,  type an  "a", enter  the word
"hello", and press ESC.  -->
When you  moved the  cursor to  the end of the line, it sat on the
last character.  When  you  pressed  "a",  it  moved  to  the next
position on the line.  You could then type in your new text.

     You can  add text  in the middle of a line too.  For example,
this line contains the word "math".  Can you add text to change it
to "mathematics"?   Try it.  If you try to do it and find that the
text is being added in  the  wrong  place,  you  started  with the
cursor in the wrong place.  Backspace over what you have typed in,
press ESC, and move the cursor so you can try again.

     There are several other commands  you  can  use  for entering
text.   The "i"  (insert) command lets you insert text IN FRONT OF
the current cursor position.  For example, move the cursor  to the
beginning of  this line.   Type  an "i",  then "hello", then press
ESC.  You will see the text INSERTED in front of the cursor.

     The "o" (open) command  creates a  blank line  after the line
that  contains  the  cursor  and  lets you append text there.  For
example, move the cursor anywhere on the next  line, then  type an
"o", then "hello", then ESC.
     You will see that a blank line is opened up when you type the
"o", and anything you type next is added to that line.

     When you use commands like "a", "i", and "o", you add text to
the material  shown on the screen.  Before you quit "vi", however,
you should SAVE these changes in a file.  To do this,  you may use
the "write" command.

     Using  ENTER  or  CTRL-D,  make  sure  that  the  entire next
paragraph is on the screen.  Now, type a colon (:).   You will see
the cursor  move to  the bottom line of the screen and display the
colon there.  Next, type a "w" (for "write"), then a space, then a
file name  like "junk".  If you make any typing mistakes while you
do this, you can correct them in the  usual way,  using backspace.
When you have the command correct, it should look something like

             :w junk

at  the  bottom  of  the  screen.   When it looks like this, press
ENTER.

     You will see that "vi" pauses for a few moments,  then prints
information on  the bottom of the screen: the name of the file and
how much text was written out.   The  "w" command  writes the text
you have  entered and  edited into  the file  called "junk".  This
saves the text so you can  use it  later.   Once you  have written
your text  into a file, it is safe to quit "vi" by typing ":q" and
pressing ENTER.  (Don't do this now!)

     If you try to  quit with  ":q" before  writing out  your text
with ":w", "vi" will check to see if you've made changes since the
last time you wrote out the file.  If you have, "vi" won't let you
quit.  This is a safety measure -- if you do not use ":w" to write
out your text before you quit "vi", all the changes  you have made
since  the  last  write  will  be  lost.  Thus "vi" stops you from
quitting if you haven't saved your recent changes.

     If you really want to quit without saving  your changes, type
":q!" and  press ENTER.   You've  probably used this while working
through this guide.  ":q!" is just an emphatic ":q".  When you are
actually working  with "vi",  you should probably use ":q" instead
of ":q!".  Leaving  off the  "!" gets  you that  safety feature we
just talked about.

     If you  want to  quit now, use ":q".  Otherwise, go on to the
next section.

      *****************************************************
      *******       PART II -- Simple Editing       *******
      *****************************************************

     We've shown you how to add  new text  to a  file.   Now we're
going to  show you  how to  change the  text that the file already
contains.

     First, we're going to look at deleting text.  The  command to
delete a  line of  text is  "dd".  Try it.  Move the cursor to the
next line and type "dd".
     TWINKLE, TWINKLE, LITTLE STAR

     You should see the line disappear on the  screen.   The other
lines on the screen will close in to fill the gap.

     To delete more than one line, type a number followed by "dd".
This deletes the given number of lines, beginning at the line that
contains the cursor.  Try it to delete the next four lines.
     TWINKLE, TWINKLE, LITTLE STAR
     HOW I WONDER WHAT YOU ARE
     UP ABOVE THE WORLD SO HIGH
     LIKE A TEA TRAY IN THE SKY

     Again, the  lines disappear  on the  screen and the remaining
lines close in.

     Now, type a "u".   You  should see  the lines  reappear.  "u"
stands  for  "undo"  and  it  reverses the effect of commands like
"dd".  This is a VERY useful command to remember.   If  you delete
something by mistake, you can get it back again if you immediately
type "u" for "undo".  However, you can  only undo  the most recent
command that  changed text.   If  you type  "u" and "u" again, the
second "u"  will "undo  the undo",  so you'll  go back  to the way
things were before the "undo".  Try it.

     Another  way  to  delete  text  is  to type "d" followed by a
command that moves the cursor.   This deletes  everything from the
beginning  cursor  position  to  where  the  cursor  ends up.  For
example, move the cursor to the  "*" in  the next  line, then type
"d$".   --> *  AND AWAY WE GO!

     You see that all the text from the "*" to the end of the line
disappears.  "$" moves the cursor to the end of the line,  so "d$"
deletes to the end of the line.

     Move the  cursor to  the beginning  of the  word BACON in the
previous line and type  "dw".   You will  see the  word disappear.
Remember that "w" moves the cursor forward a word, so "dw" deletes
everything from the cursor to  the  beginning  of  the  next word.
What will "db" do?  Try it on a word in this line.

     You can  even use  searches after  "d".  Type a "/" or "?" to
begin the search, then the pattern you want to search  for, then a
closing  "/"  or  "?"  (whichever  you started with).  When you're
done, press ENTER.   "vi" will  search forward  or backward  for a
string that matches the desired pattern and delete everything from
the cursor to  the  matching  string.    Move  the  cursor  to the
beginning of the next line and type
                    d/hello/
As you see, this deletes everything up to the "hello".

     If you  want to delete a single character, move the cursor on
top of the character and type "x" (lower case).   Use this  to get
rid of the double quotes around "x" in this sentence.

     The "cc"  command changes a line.  To see how it works, begin
by getting this whole paragraph on the screen.  Move the cursor to
any part of the next line and type "cc".
          THE RAIN IN SPAIN FALLS MAINLY ON THE PLAIN.
You will see the whole line disappear.  The cursor may also change
into a block.  You are now in  Insert Mode  and anything  you type
will be  displayed on  the screen.  Type "hello", press ENTER, and
type "hello again".  You will see the two lines put in  to replace
the one old one.  To get out of Insert Mode, press ESC.

     If you  type a  number followed by "cc", "vi" will get rid of
that number of lines and let you type in new text to replace them.
For example,  move the  cursor to  the first  of the new lines you
entered in the last paragraph and type "2cc".   "vi"  replaces the
two lines with a single blank line.  Just press ESC and that blank
line will be all that is there.

     A "c" followed by a  command  to  move  the  cursor  lets you
change everything  from the  place where  the cursor starts to the
place it ends up.  To see how  this works,  begin by  getting this
whole  paragraph  on  the  screen.    Move the cursor to the first
letter of the word BACON on this line and type "cw".  You will see
that "vi"  puts a  "$" sign  on the last letter of the word.  This
marks the end of the material that will be changed by the command.
The cursor  will still  be at the beginning of the word.  Now type
"VEGETABLE" and press ESC.  You see that  the characters  that you
type print  over the  characters that  were there until you get to
the "$" sign.  Then the rest of the line  moves over  to make room
for the extra characters that you add to the line.

     Whenever you  are changing  text with  a "c" command, you can
enter as much or as little new text as you want.  For example, you
could have  replaced BACON  with many  lines of text if you liked.
The rest of the text would  have moved  to make  room for  the new
text you entered.

     If you  want to  change a single character in your text, move
the cursor to that  character, type  "r" (for  "replace") and then
the  new  character.    You  do  not  have  to  type ESC.  The new
character will replace the old one.

     By the way, the "u" command  undoes "c"  and "r"  commands as
well as  "d".   For example, move the cursor to the X on this line
and type "rY" to change the X to a Y.   If  you now  type "u", you
will see the Y change to a X.  Keep pressing "u", and you will see
the character flip-flop back and forth between X and Y.

      *****************************************************
      *******    PART  III -- Moving Text Around    *******
      *****************************************************

     One common editing job is moving text from one part of a file
to  another.    For  example,  you  may want to move a sentence or
paragraph from one section of a report to  another.   You may want
to move  whole sections  around.  You may also want to copy chunks
of text and then modify them somewhat.   For example,  I just made
the heading  for this  Part of  the guide  by making a copy of the
heading for Part II and editing  the  title  that  was  inside the
stars.   And, of course, I made the heading for Part II by copying
the heading for Part I and  editing that.   There  are repetitious
aspects to  most word  processing jobs,  and you can save yourself
some typing using the copy-and-edit process.

     There are two commands involved in the copy-and-edit process.
The  first  is  the  "y"  command,  also  called "Yank".  The Yank
command makes a copy of a block of  text.   It works  like "c" and
"d": you move the cursor to the beginning of the block you want to
copy, then type "y" plus a cursor movement  command that  moves to
the end of the block you want to copy.  For example, here's a line
of text:
        LITTLE BOY BLUE, COME BLOW YOUR HORN
Move the cursor to the beginning of the line and type "y$".   This
makes a  copy of  the text  from the cursor position to the end of
the line.  Not much happens when you make the copy -- you may hear
the disk do some writing and the screen may blink once, but that's
about it.  The text you just copied stays where  it is.   Now move
the cursor to the empty line following this line and type "p".

     "p" stands for "Pull", and it pulls copied text back into the
file you're editing.  The text  is written  down immediately AFTER
the cursor.  Any text that was there already moves to make way for
the next text.  Now move the cursor to the  beginning of  the next
blank line and type "p" again.

     As you  can see,  typing "p"  pulls in  the same line of text
again.  You can  pull  in  the  same  text  any  number  of times.
However, if  you use "y" to copy/yank some new text, "p" will pull
in the new text instead of the old.

     If you want to yank an entire line,  put the  cursor anywhere
on  the  line  and  type  "yy".   (You have already seen that "cc"
changes a line and "dd" deletes one.)    If  you  want  to  yank a
number of lines, type the number and then "yy".  For example, move
the cursor to the beginning  of  this  paragraph  and  type "8yy".
This will yank the eight lines of this paragraph.  Move the cursor
to the next blank  line and  type "p".   This  pulls in  the whole
paragraph.

     Up to  this point,  we have  had you yanking text by going to
the beginning of what you want to copy and typing  "y" followed by
a cursor movement command that went to the end of what you want to
copy.  "vi" lets you do the opposite too.   For example,  move the
cursor to  the end  of this paragraph and type "yH".  You will see
the cursor jump to the first line of the screen (which is  what it
does when you type "H" for High).  You have just yanked everything
to the top of the  screen.    Go  to  the  blank  line  after this
paragraph and type "p" to pull in what you just yanked.

     Now suppose  you want  to copy  a particular sentence.  To do
this, you put the cursor at the beginning or end  of the sentence,
then type  "y" followed  by a cursor movement command that goes to
the end or  beginning  of  the  sentence.    What  kind  of cursor
movement command  would this  be?   Well, you could enter a search
command that looked for the first  or last  word of  the sentence,
but  that's  a  bit  clumsy...and  what  happens  if  that word is
somewhere in the middle of the sentence too?

     To deal with problems like  this,  "vi"  has  a  command that
moves the  cursor to the beginning of a sentence.  This is the "("
command.  For example, move the cursor to the X  in this  line and
type the  "(".  You will see the cursor move back to the beginning
of the sentence.  Move the  cursor to  the beginning  of THIS line
and type  "(".   You will see the cursor jump back one line to the
beginning of the sentence.  So yanking a  sentence is  easy.  Move
to the  end of  this very sentence and type "y(".  The cursor will
jump back to the first word of the sentence.  Now move  the cursor
to the next blank line and type "p" to see what you've yanked.

     You will see that the word "Move" is on one line and the rest
of the sentence on  the next  line.   This is  because the yanking
process copies the "line break" after "Move".  If you want to make
things prettier, move the cursor to the word "Move" and type "J".
"J" stands for "Join" and it joins two lines in  the way  you just
saw.   The cursor can go anywhere on the first line; when you type
"J", the next line is added one the end of the line that holds the
cursor.

     By the  way, we should point out what happens if you type the
"(" command when the cursor  is  already  at  the  beginning  of a
sentence.   Move the  cursor to  the X  in this line and type "(".
The cursor jumps to the  beginning  of  the  sentence.    Type "("
again,  and  the  cursor  jumps  to  the beginning of the previous
sentence.  So "(" can be used to go backward sentence by sentence.

      *****************************************************
      ******* PART IV -- Contextual Cursor Movement *******
      *****************************************************

     The  "("  command  is  one  example  of  "contextual"  cursor
movement.   It moves the cursor to a position that is important in
the context of your text.  There are several others.

     ")" goes to the  beginning of  the next  sentence.   Move the
cursor to  the beginning  of the previous line and type ")" to see
what happens.  Now type it again.

     "{" (the opening brace bracket) goes back to the beginning of
a  paragraph.    Move  the  cursor  to  the  end  of the preceding
paragraph and type "{" to see what happens.  Now what do you think
will happen if you type "}"?  Try it and find out.

     The  "(",  ")",  "{",  and  "}"  commands  all  use technical
definitions for  what a  "sentence" and/or  "paragraph" look like.
These  definitions  are  given  in  the Reference Manual entry for
"vi".  Once  in  a  while,  you  and  "vi"  may  disagree  on what
constitutes a sentence, but most of the time, "vi" knows what it's
doing.

      *****************************************************
      *******       PART V -- Editing Options       *******
      *****************************************************

     "vi" has many options that change the way  the editor behaves
during  an  editing  session.    These  Toolkit  Reference  Manual
contains a complete list of these options.   In this  guide, we'll
only discuss a few that may be immediately useful.

     You  must  be  in  Command  Mode  to  set  options  (i.e. not
appending text).  To set an option, begin  by typing  a colon (:).
You will  see the  cursor move  to the bottom of the screen.  Then
type the word "set", a space, and the name of the option  you want
to  set  --  we'll  talk  about  option names in a moment.  Typing
mistakes can  be corrected  by backspacing.   When  you have typed
everything correctly, press ENTER.

     One commonly used option is "ignorecase".  If you type

            :set ignorecase

"vi" will not pay attention to the case of letters when searching.
For example, type in  the above  "set" command,  then move  to the
beginning of  this paragraph  and type "/for" (followed by ENTER).
You will see that the cursor moves to  the "For"  at the beginning
of the  previous sentence, even though the "F" is upper case.  Now
type "/" followed by ENTER.   This time,  you see  that the search
finds the  word "for"  with the  "f" in  lower case.   Many people
prefer "caseless" searches over  "case-sensitive"  ones.    If you
want to go back to case-sensitive searches, type

            :set noignorecase

     By default, "vi" sets tab stops every 8 spaces.  For example,
if you begin a paragraph by typing a tab, the tab moves the cursor
over  8  spaces.    If  you  are entering a program using tabs for
indentation, each level of indentation will be 8 spaces.  Move the
cursor to the next blank line, type "a" to append text, then a tab
and "Hello!" to see how much space is created for the tab.   Press
the ESC key to leave Insert Mode.

     Many people  feel 8  spaces is  too big  for a tab stop.  For
example, in this guide, we have chosen to  indent paragraphs  by 5
spaces (not using tabs).  You can set tab stops of 5 spaces with

            :set tabstop=5

When  you  press  ENTER,  keep  your  eye on the "Hello!" line you
entered a few lines back.  You should see the  word jump backwards
from column  8 to column 5.  All tab stops in the file are changed
with the one command.  (NOTE:  What you  are changing  is how "vi"
DISPLAYS tabs.   You  are not  changing the  contents of the file.
The file just contains a tab character.)  Similar commands can set
tabstops to any number of spaces.

      *****************************************************
      *******   PART  VI -- Editing Several Files   *******
      *****************************************************

     In  a  typical  "vi"  session,  you  may want to edit several
files.  When you have finished  editing one  file, you  must first
write out  the text.   If  you do  not save the text you have been
editing, any editing you have done  will  be  lost  when  you quit
"vi".  If you started your "vi" session with the command

                 vi filename

you can save your text in that file just by typing

                 :w

and pressing  ENTER.  You can try this now IF YOU ARE WORKING WITH
A COPY OF THE  VI GUIDE.   (If  you are  not working  with a copy,
don't do it, or you'll write over the original file.)

     If you want to save your text in a new file, you can type

                 :w newfile

where "newfile" is the new file name.  Try this now.  Type

                 :w junk

and press  ENTER.   When "vi"  has finished  writing the file, you
will see that it prints the name of the file  and some information
about the file's size.

     If you  want to  save your text in a file that already exists
but which was not the file you were  originally editing,  you have
to type

                 :w! file

The exclamation  point indicates that you know the write operation
will overwrite the old contents of the file and that you want this
to happen.   (Don't  try this unless you have some files you don't
mind writing over.)

     Once you have written out your changes, you can start editing
a different file by typing

                 :edit newfile

then pressing  ENTER.   This will clear out the text you have been
editing and set things up so you can  edit the  new file.   If the
file already exists, its current contents will be read in.

     Now here's  a trick  we can't  demonstrate in this guide, but
it's something to remember when  you  want  to  edit  a  number of
files.  If you start "vi" with a command line of the form

                 vi file1 file2 file3 ...

you can  edit several  files one  after the other.  After you have
finished editing a file AND have written it  out (with  ":w"), you
can start editing the next file by typing

                 :next

or just

                 :n

It may  be particularly  useful to use wild card characters on the
"vi" command line, as in

                 vi *.c

As described in Chapter 2 of  the  Overview  Guide,  this  will be
expanded to a list of all the files under the current catalog that
have the ".c" extension.

      *****************************************************
      *******        PART  VII -- Conclusion        *******
      *****************************************************

     We have now introduced the  basics  of  entering  and editing
text with  "vi".  You should be well-equipped to cope with most of
the operations you want to do with "vi".  Programmers  should also
look at  the "program.v"  to read  about the  special features for
entering source code.  Those using "vi" to create documentation of
various types may want to look at "doc.v".

     We  now  recommend  that  you  experiment with "vi", creating
small files of your own and editing them.   Once  you are familiar
with the  parts of the editor that this guide has discussed, begin
reading bits of the  complete description  of "vi"  in the Toolkit
Reference Manual.   "vi"  has many many features; some of them may
be useful in the work you do, while others are not.  Just remember
that "vi" has something for everyone.  Good luck!

