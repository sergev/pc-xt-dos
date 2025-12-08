      E D I T I N G   P R O G R A M   S O U R C E   C O D E
                         U S I N G   V I

     In  this  file,  we  discuss  a number of commands of special
interest when entering or editing program source code.   We assume
that you have already read the material in "browse.v" and "edit.v"
describing commands for entering text and  editing files.   If you
have  not  worked  through  these  files,  you should do so before
reading this.   Also,  you should  be working  with a  COPY of the
"program.v" file, not the original.   The introduction to the "vi"
guide (on paper with the other tutorial guides) tells how  to make
copies of your files.

     The  "browse.v"  file  explained  how  to read through a file
using "vi".  If we don't  give any  instructions to  the contrary,
just read forward through this file by pressing ENTER or CTRL-D.

     This part  of the  guide is  intended for  those who will use
"vi" to enter program source code.  Since "vi" originated on UNIX,
the  editor  contains  a  number  of  features  primarily aimed at
programming in the C language.   However, these  same features are
applicable to  many other  languages, particularly structured ones
like Pascal, Modula-2, and Ada.

      *****************************************************
      *******     PART I -- Indentation Control     *******
      *****************************************************

     The source code for a program differs from "ordinary" text in
a number  of ways.   One of the most important of these is the way
in which source code  uses  indentation.    Indentation  shows the
logical structure  of the program: the way in which statements are
grouped into blocks.

     Issue the command

                 :set autoindent

(Don't forget to press ENTER after  you  have  typed  this.)   The
command  turns  on  a  "vi"  option  primarily supplied to control
indentation when entering source code.  To see how it  works, copy
(append) the next two lines in the blank line that follows them.
        if (a > b) max = a;
        else max = b;

(Make sure  you add  the blank space at the beginning of the first
line, either with a  number of  spaces or  by pressing  TAB.)  You
will notice that when you press ENTER after typing the first line,
the cursor  goes to  the next  line and  automatically indents the
same distance as the previous line.

     This is  the meaning of the "autoindent" option: each line is
automatically indented the same distance as the previous  one.  As
a programmer,  you will  find this  saves you  quite a bit of work
getting the  indentation right,  especially when  you have several
levels of indentation.

     When you  are entering code with "autoindent" enabled, typing
CTRL-T gives you another  level of  indentation and  typing CTRL-D
takes one away.  For example, copy (append) the next four lines in
the blank line that follows them.
        To indent the next line, type CTRL-T at start of line.
                Next line will be indented this same distance.
                On next line, type CTRL-D at start of line.
        Notice how CTRL-D moves cursor back one level of indent.

     We should point out  that CTRL-T  and CTRL-D  are typed while
you are  in Insert Mode, unlike most other commands that are typed
in Command Mode.

     The amount of  indentation  provided  by  CTRL-T  is  one tab
character.  As we discussed in "edit.v", tab stops are set every 8
spaces by default.  A command like

                 :set tabstop=4

can change the tab settings for a file.

     Try using  the  "autoindent"  option  when  you  are entering
source  code.    It  simplifies  the  job  of  getting indentation
correct.  It can  even sometimes  help you  avoid bugs  (e.g. in C
source  code,  where  you  usually  need one closing "}" for every
level of indentation you go backwards).

     The "<<" and ">>" commands are  also  helpful  when indenting
source code.   By default, ">>" shifts a line right 8 spaces (i.e.
adds 8 spaces  of indentation); "<<" shifts a  line left 8 spaces.
For example, move the  cursor to  the beginning  of this  line and
press  the  ">" key twice.  You will see  the line move right.  If
you now press the "<" key twice, the line will move back again.

     You can shift a number of lines by typing the number followed
by ">>" or "<<". For example, move the cursor to the first line of
this paragraph and type "5>>".  You will  shift all  five lines in
the paragraph.  What command will move the paragraph back?  Try it
to make sure.

     The default shift is 8 spaces (right or left).   This default
can be changed with a command like

                 :set shiftwidth=4

You will  find it  convenient to  have a  "shiftwidth" that is the
same size as the width between tab stops.

      *****************************************************
      *******  PART II -- A Special Search Command  *******
      *****************************************************

     The characters "(", "[",  "{",  and  "<"  can  all  be called
opening  brackets.    When  the  cursor is resting on one of these
characters, pressing the "%" key moves the cursor from the opening
bracket  forward  to  the  corresponding closing bracket ")", "]",
"}", or ">", keeping in mind the usual rules for nesting brackets.
For example, move the cursor to the first "(" in

              if ( cos(a[i]) > sin(b[i]+c[i]) )
              {
                  printf("cos and sin equal!");
              }

and  press  "%".    You  will  see  that  the  cursor jumps to the
parenthesis  at  the  end  of  the  line.    This  is  the closing
parenthesis that matches the opening one.

     Similarly  if  the  cursor  is  on one of the closing bracket
characters, pressing "%" will move  the  cursor  backwards  to the
corresponding opening  bracket character.   For  example, move the
cursor to the closing brace bracket after the  "printf" line above
and press "%".

     Not only does this search character help you move forward and
backward through a program in long  jumps, it  lets you  check the
nesting of  parentheses in  source code.   For example, if you put
the cursor  on the  first "{"  at the  beginning of  a C function,
pressing "%"  should move you to the "}" that (you think) ends the
function.  If it doesn't, something has gone wrong somewhere.

      *****************************************************
      *******    PART III -- Large Scale Changes    *******
      *****************************************************

     If the name of a data object or function has to be changed in
a program  (for whatever  reason), it  becomes necessary to change
every occurrence of that name.   This would  be a  tedious process
using the  "vi" features we have discussed up to this point, since
you would have to search through each source file for the name and
then type  in the  new name  wherever the  old one  was found.  To
avoid much of this work, "vi" offers the "substitute" command.

     The usual form of the substitute command is

                 :s/pattern/replacement/

where "pattern" is any of the patterns used in searches (discussed
in "browse.v")  and "replacement" is any string.  For example, get
the rest of this paragraph on the screen, then move  the cursor to
THIS line and type

                 :s/THIS/this/

As soon  as you type the colon (:), you see the cursor move to the
bottom of  the screen.   Type  the rest  of the  command and press
ENTER.   You will  see "THIS"  turn into "this".  The command puts
the given "replacement" string  in the  place of  the first string
string that matches the given "pattern".

     What happens  if a line has more than one string that matches
the pattern?  This line contains the word "the" several times.  If
you move the cursor to the line and type

                 :s/the/XXX/

what happens?   Only the first match is replaced.  The position of
the cursor in the line does not matter.

     If you want to change every occurrence of a string on a line,
type  a  "g"  (standing  for  "global") after the last slash.  For
example, move the cursor to this line and type

                 :s/h/H/g

You will see both "h" characters change.

     Now how does the "s" command  help make  large scale changes?
In addition  to applying "s" to a single line, you can apply it to
a RANGE of lines.   For  example, type  the following  command and
press ENTER.

                 :1,200s/^/!/

What happens?   The "1,200" in front of the "s" indicates that the
command  should  be  applied  to  the  lines  from  1  through 200
(everything up  to the  200th line  in the file).  The "s" command
itself says that the beginning of the line (^) should  be replaced
by an  exclamation point.   So (as you have probably seen by now),
an exclamation point has been put at the  beginning of  every line
up to number 200.  To get rid of the exclamation points, type

                 :1,200s/^!//

(which  says  change  every  "!"  at  the beginning of a line into
"nothing").

     Now, in the above instructions, we  made use  of line numbers
to refer  to lines.   How  do you know what number a line has?  If
you just want to know the number of one line,  move the  cursor to
that line  and type  ":.=".   For example,  find out what the line
number of THIS line is.  For another approach, type

                 :set number

and press ENTER.  As you can now see, this displays the  number of
every line in the file.  If you want the numbers to go away, type

                 :set nonumber

It's up to you whether or not you want to keep the numbers.

     There are  a number  of special symbols that can be used when
specifying a range of lines.   The "."  stands for  the line where
the cursor  is currently positioned.  For example, move the cursor
to this line and type

                 :1,.s/$/???/

This adds "???" to the end  of every  line from  the start  of the
file to  the line  containing the cursor.  Move the cursor down to
this line and type

                 :1,.s/???$//

This removes the question marks.  Notice that  the command worked,
even though  the last  few lines  before the cursor did not end in
"???".  When you issue a "substitute" command with a  range, it is
all right  if some  of the  lines in  the range do not contain the
pattern you are replacing.

     When specifying  a range  of lines,  "$" stands  for the last
line in the file.  Try this command

                 :1,$s/the/THE/g

What does it do?  It changes every "the" in the file to upper case
(including words like  "there"  where  "the"  is  part  of another
word).   Now, just  to put things back the way they were, type "u"
to undo the change you just made.  This demonstrates  that the "u"
command can undo changes made by ":s" as well as by "c" and "d".

     It should  be obvious  now how  to make  a global change in a
file.  To give an example, here's a small piece  of program source
code to change.

     int bubble()
     {
         extern int array[30];
         int i, j, temp;
         for (i = 0; i < 30; i++)
             for (j = 29; j > i; j--)
                 if (array[j] < array[j-1])
                 {
                     temp = array[j];
                     array[j] = array[j-1];
                     array[j-1] = temp;
                 }
         return array[0];
     }

Now, use an "s" command to change every occurrence of "array" into
"list".  Make sure you get the line  range correctly  -- use ":.="
to find out the line numbers of the first and last lines if you no
longer have the line numbers shown on your  screen.   You can also
use

                 :set number

to make the line numbers reappear on your screen.

     Now  check  the  sample  source  code  to  make sure that you
changed every occurrence of "array" into "list".   The command you
should have used to do this was

                 :254,267s/array/list/g

If you  forgot the  "g" on the end, the lines that contain "array"
twice will still have references to "array".  Without the "g", the
"s"  command  only  changes  the  FIRST  occurrence  of  the given
pattern.

     What would you do now if you want to change  the variable "i"
into a "k"?  You can't just use an instruction like

                 :254,267s/i/k/g

because that  will change  the letter  "i" into  "k" even in other
words like "int" and "list".  The solution to this is to add a "c"
(for Check)  after the "s" command.  When you do this, "vi" checks
with you before making every substitution.   Before  each possible
change, "vi" prints the line at the bottom of your screen and puts
a "^" under the string that  might be  changed.   If you  want the
change to  happen, press the "y" key followed by ENTER.  If you do
not want the change to  happen,  press  the  "n"  key  followed by
ENTER.  To see how this works, try

                 :254,267s/i/k/gc

and only  change those  occurrences of "i" that are related to the
variable "i".

     Using the ":s" is much  faster  (and  less  error-prone) than
trying to  change every  occurrence of a symbol by hand.  Remember
the "c" option, if you think  there's a  chance that  the name you
are trying to change might be part of a larger symbol.

      *****************************************************
      *******     PART IV -- Executing Commands     *******
      *****************************************************

     Suppose you  have just  finished creating  a source code file
using "vi" and you are ready to compile it.  You can write out the
file, quit "vi", and then compile it if you want...but we all know
that there are likely to be compilation errors the first  time you
compile a  new file.   To  fix these  compilation errors, you will
have to use "vi" again to edit the source code.

     To avoid the wasted effort of quitting "vi"  to compile, then
going back  into "vi"  to fix  errors, you  can invoke the compile
command directly from "vi".  First, write out your source  code to
a file by saying

                 :w filename

(You don't  have to specify the filename if you've already written
to the file once before, or if you read the text in  from the file
originally.)   Now, type  a colon  (:), followed by an exclamation
point (!), followed by  the  command  you  would  normally  use to
compile that file, as in

                 :!msc filename

(if you are using the Microsoft C compiler).  Press ENTER when you
have typed the line.  This will compile  the program,  and display
any diagnostic  messages in  the usual way.  After the compilation
is finished, you return to "vi" where you  can correct  any errors
that might have been detected.

     Other commands can be executed in the same way.  For example,
in "browse.v" we showed how to use the "help" command in

                 :!help vi

You can also use commands like

                 :!pg file

if you  want to  look up  information in  some other file (e.g. to
check another source code file  for  the  name  of  a  function or
variable).

      *****************************************************
      *******         PART  V -- Using Tags         *******
      *****************************************************

     This is  the last  section of "program.v".  It is of interest
to C programmers, but not to those working in other  languages.  C
programmers will  find it  useful to create a copy of one of their
existing C source files.  You  can  do  this  right  now  with the
command

                 :!cp file.c junkfile.c

where  "file.c"  is  the  name  of  one of your C source files and
"junkfile.c" is the version of the file we will work on.

     The source code for a large C program will usually  be spread
over several  files.   Sometimes, it is difficult to keep track of
which file  contains  which  function  definitions.    To simplify
matters,  the  Toolkit  offers  the  "ctags" command and the "tag"
command of "vi".

     The "ctags" command is issued at  the shell  level (either to
the  "sh"  shell  supplied  with  the  Toolkit,  or  to  the usual
COMMAND.COM of DOS).  Its purpose is to create an information file
that  "vi"  can  use  later  to determine which files define which
functions.  A command of the form

                 :!ctags junkfile.c

creates a file named  "tags" under  your current  directory.  This
contains information  on the functions defined in "junkfile.c".  A
command like

                 :!ctags *.c

creates a "tags" file describing ALL the C source files  under the
directory.

     Now suppose  your "tags" file contains information on all the
source files that make up a C program.  Also suppose that you want
to look at or edit a function in the program but do not know where
the function is.  The command (don't type this now!)

                 :tag NAME

will look at the "tags" file to find  out which  file contains the
definition of  NAME.   It will  then read in the file and position
the cursor on the line where the name  is defined.   In  this way,
you don't  have to know which file you have to edit; you only have
to decide which function you want to edit.

     NOTE: when you use the ":tag" command to move to  a new file,
"vi" will  normally discard  the text you were previously editing.
Of course, this means that the text will be lost if it hasn't been
saved in  a file.   For  this reason,  if you try to use ":tag" to
read in a new file and you haven't  saved your  current text since
the last  time you  saved it,  "vi" will not let you go to the new
file.  You must either write out your  current text  with the ":w"
command and then issue ":tag", or else type

                 :tag! NAME

     You are  now finished  the "program.v" Guide.  If you want to
see how "tag" works, try

                 :tag NAME

where NAME is one of the functions defined in "junkfile.c".   From
here on  out, you  should know  everything you  need to create and
edit programs.  Good luck!

