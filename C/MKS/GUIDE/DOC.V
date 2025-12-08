	      P R E P A R I N G   D O C U M E N T S
			 U S I N G   V I

     In  this  file,  we  discuss  a number of commands of special
interest when entering or editing documents.   We  assume that you
have  already   read  the  material  in  "browse.v"  and  "edit.v"
describing commands for entering text and  editing files.   If you
have  not  worked  through  these  files,  you should do so before
reading this.   Also,  you should  be working  with a  COPY of the
"doc.v" file,  not the  original.    The  introduction to the "vi"
guide (on paper with the other tutorial guides) tells how  to make
copies of your files.

     The  "browse.v"  file  explained  how  to read through a file
using "vi".  If we don't  give any  instructions to  the contrary,
just read forward through this file by pressing ENTER or CTRL-D.
 
     This part  of the  guide is  intended for  those who will use
"vi" to  enter  documents:  reports,  letters,  etc.    Since "vi"
originated  on  UNIX,  the  editor  contains  a number of features
related to the use of "troff", a program that can format documents
for printing  on various  kinds of  machines, e.g. laser printers.
The MKS Toolkit  also  has  the  "fmt"  command  which  can format
documents  for  printing  on  a  line  printer  or on a typewriter
terminal.  Lastly, "vi" has features which let  you prepare output
for  printing  without  any  other formatting software.  This file
will begin with  features  that  are  useful  to  anyone preparing
documents,  and  will  then  move  on  to  hints  for  those using
different kinds of formatters.

      *****************************************************
      *******        PART  I -- General Tips        *******
      *****************************************************

     When you are typing in lines of text in paragraph  form, your
job is  easier if  you don't  have to remember to press ENTER when
you reach the end of a line.  Try entering the following command.

	    :set wrapmargin=8

Now go to the next blank line, type  "a" to  begin appending text,
and  start  typing  out  the contents of this paragraph, beginning
with "When you...".  DON'T press ENTER when you start  copying the
second line.  Keep on going.  Press ESC to leave Insert Mode after
a few lines.

     You should see that  the  word  "easier"  automatically jumps
down to  a new  line.   The option "wrapmargin=8" says that if you
end a word within 8 spaces of the right hand margin of the screen,
the next word should "wrap around" to begin on the next line.  You
do not have to press ENTER.  With this kind of Wrap-around Margin,
you can  just keep  on typing  words.   The only  time you need to
press ENTER will probably be at the end of paragraphs.

     "vi" lets  you define  abbreviations for  commonly used words
and phrases.   This  uses the  ":ab" (abbreviate)  command.  As an
example, try typing

		 :ab mks Mortice Kern Systems Inc.

and  then  press  ENTER.    This  says  that  "mks"  will   be  an
abbreviation  for  "Mortice  Kern  Systems  Inc.".   To see how it
works, move the cursor to the next blank line, press  "a" to begin
appending text, and type
       "vi" comes with the mks Toolkit.

You will  see that  as soon as you type the space after "mks", the
abbreviation is expanded into the associated phrase.  Now move the
cursor to  the next blank line, press "a" to begin appending text,
and type
       The MKS Toolkit contains "vi".

This time, you will  see  that  "MKS"  is  not  expanded  into the
abbreviation.  The abbreviation feature pays attention to the case
of letters.  Now move the cursor to the next blank line, press "a"
to begin appending text, and type
       mkswill not expand.

When you  type this,  you will  see that the "mks" is not expanded
because  it  is  part  of  another  "word".    "vi"  only  expands
abbreviations when they are not part of other words.

     An  abbreviation  only  lasts  for  the  duration  of  a "vi"
session.  If you set up an abbreviation now, it will be remembered
as long  as you stay in "vi", but will be forgotten once you leave
"vi".  This means  that you  have to  set up  common abbreviations
every time you start "vi".  This would be a lot of work if you did
it by hand, but you can simplify things quite a bit by  setting up
a "command file".

     A  command  file  contains  a  number of commands that can be
executed as if they were typed  in a  "vi" session.   For example,
you might use "vi" to create a file with the contents

		    set wrapmargin=8
		    set tabstop=5
		    ab mks Mortice Kern Systems Inc.
		    ab aa another abbreviation
		    ab bb another abbreviation
			 ...and so on...

setting all  the options you want to use and all the abbreviations
you commonly need.   The file  can only  contain instructions that
normally start  with a  colon (:) in "vi", and you omit the colons
in the command file.  You can execute all the  instructions in the
command file inside "vi" with the instruction

		    :so filename

where "filename"  is the  name of  your command file.  "so" stands
for "source" and it tells "vi" that the given file should be taken
as the source of a number of commands.  To see how it works, type

		    :so /guide/source

and press  ENTER.   You should  see the message "Hi there" printed
out on your screen.  This was done by an instruction in the file.

     As a matter of  fact,  you  can  execute  the  commands  in a
command  file  when  you  first  start  "vi".  Start "vi" with the
command line

		    vi "+so cmdfile" editfile

where "cmdfile" is the name of your command file and "editfile" is
the name  of the file you want to edit.  If you are using the Korn
Shell (described in Chapter 6 of the Toolkit  overview), you might
want to set up an alias for

		    vi "+so cmdfile"

     If you  want to get rid of an abbreviation that has been set,
use the ":una" (unabbreviate) command.  For example, type

		    :una mks

This gets rid of the "mks" abbreviation we set up earlier.

      *****************************************************
      *******      PART  II -- Combining Files      *******
      *****************************************************

     Occasionally, you may want to combine a number  of files into
a single  document.   For example,  you may  have a  table of data
stored in one file and wish to add the table to another  file.  To
demonstrate  how  this  can  be  done, move the cursor to the next
blank line and type
		    :r /guide/data

You should see the  contents of  the file  read in  after the line
that holds  the cursor.   "r"  stands for  Read, and  it reads the
contents of a file, to be added to the current file after the line
indicated by the cursor.

     The same  sort of command may be used to combine the chapters
of a document into a single file.  For example,

		    :r chapter1
		    G
		    :r chapter2
		    G
		    :r chapter3

will read in chapters that are stored in  separate files.   Notice
that we had to add "G" commands to go to the end of the file after
each read operation, so that the next input file would be added to
the end of the text.

     We might  note that  it is  a very  good idea  to store large
documents in a number of smaller files and only combine  them when
you are  ready to  create the  finished product.   The smaller the
file, the faster "vi" can read  and write  the text.   Small files
are also more convenient for you to edit, since it takes less time
to find the text you want to change.

      *****************************************************
      *******       PART  III -- Sorting Data       *******
      *****************************************************

     In many types of reports, it is convenient to be able to sort
a few  lines of  text (e.g.  for a  table of  information).  As an
example, consider the following lines.

	  a   -- append after cursor
	  o   -- open an input line
	  dd  -- delete a line
	  cc  -- change a line
	  w   -- move forward a word
	  b   -- move backward a word
	  x   -- delete a letter
	  u   -- undo

I entered these descriptions  of  "vi"  commands  in  the  order I
thought of them, but it would be nice to sort them in alphabetical
order.  To do this, we can send  the the  above lines  through the
"sort" command (described in Chapter 4 of the Toolkit Overview).

     First, we  have to  find out the line numbers of the lines we
want to sort.  To do this, move the cursor  to the  first line and
type ":.="  (colon, dot, equals).  "vi" will print the line number
(which should be 184).  Move the cursor to the last line  and type
the same  thing.  You should find out the line number is 191.  Now
type

		  :184,191!sort

and press ENTER.  This asks "vi" to send lines 184 to  191 through
the "sort" command.  The lines will be replaced with the output of
the sort command, namely the sorted lines.  Go back  and check the
list, and you will see that it is now sorted.

     The same  technique can  be used  to send lines through other
Toolkit commands if you like.  For example, a number  of lines can
be  put  into  columns  using  the "c" command.  Remember that the
output of the command  you call  will replace  the lines  that are
originally there.

     If you  do not  specify a  range of  lines for a command, the
command will just be executed.   Its output  will be  displayed on
the screen, but will not replace any of the text of the file.  For
example, write out this file and then try the command

		 :!wc doc.v

As described in the Overview  Guide,  "wc"  prints  the  number of
lines, words, and characters in a text file.  In other words, this
command gives you a quick word count for your document.

      *****************************************************
      *******     PART IV -- Formatting by Hand     *******
      *****************************************************

     Formatting text by hand  can be  tricky, especially  for long
documents.   However, for short pieces like letters, there usually
isn't much problem.

     The CTRL-L  character is  called the  "formfeed".   Use it to
separate pages.  For example, on the next blank line, press "a" to
begin appending text, then enter CTRL-L  by holding  down the CTRL
key and  pressing the  L key.   After  this, press  the ESC key to
leave Insert Mode.

     You should see that the character appears as  "^L".   This is
the form  that "vi"  uses to indicate CTRL characters.  By putting
in these formfeed characters, you  can  separate  a  document into
pages.

     On a normal typewriter, an 8 1/2 by 11 sheet of paper holds a
maximum of 66 lines.  For the purpose of formatting,  54 lines fit
nicely on  a page,  with an  inch of  space at  top and bottom for
margins.  Thus you  should  consider  putting  formfeeds  every 54
lines (give or take one or two).

     If you indent a paragraph with a tab character, the amount of
indentation will be  determined  by  the  tab  stops  set  for the
printer  on  which  the  document  is  printed.    If you indent a
paragraph with spaces, the  amount of  indentation will  always be
the same.

     The ">"  command can be used to indent a number of lines in a
uniform way.  For example, try typing

                 :1,260>

and press ENTER.  This indents the first  260 lines  of this file.
You can get rid of the indentation by typing

                 :1,260<

and  pressing  ENTER.    As  an  instruction to try later, you can
indent the entire file with

                 :1,$>

The "$" is a special symbol that stands for the  last line  of the
file (when  you are  giving a  range of  lines to a command).  The
reason we suggest you try this later is that the  cursor will move
to the  last line  indented (the end of the file) which would lose
your place in this file.

     Once you have formatted  your file,  write it  out to  a file
(with ":w").  You can then try printing the file by typing

                 :!print filename

and pressing  ENTER.   This calls the DOS "print" command to print
your file.

      *****************************************************
      *******          PART V -- Using Fmt          *******
      *****************************************************

     The "fmt" command is  a simple  text formatter.   Unlike more
complicated  formatters  like  "troff",  "fmt"  has  no formatting
requests.  Its great virtue is its simplicity.

     If you plan to put text through "fmt", here are some rules to
follow.

     (a)  Start  every  sentence  on  a new line.  "fmt" will
          concatenate your sentences  and  fill  them  out to
          give you an even right hand margin.  You don't need
          to  worry  about   getting   the   margin  straight
          yourself.

     (b)  Separate paragraphs  with a  blank line.  The blank
          line tells "fmt" that a new  paragraph is starting.
          It will be appropriately indented.

XXX: To see how "fmt" works, here is a sample of "fmt" input.
These lines have a "ragged right", which means that we have no
justification on the right hand margin.
You will also notice that every sentence starts on a new line,
even though some sentences are broken over several lines.

This is the start of another paragraph.
It will show you how "fmt" breaks text into paragraphs.
The sample "fmt" input ends here. :XXX

     Now, let's run the previous lines through "fmt".  To do this,
find out the line numbers of the first and last lines using ":.=".
You should find that they are 310 and 318.  If you now say

                 :310,318!fmt -j

the lines  should be  replaced by  text nicely formatted by "fmt".
The text now has a justified right margin  and looks  more or less
like the  rest of the text in this guide (although the line length
is different).

     "fmt" also breaks your  text  up  into  pages.    For further
information, see the description of "fmt" in the Toolkit Reference
Manual.  It's quick and easy to use, and suitable for many day-to-
day text preparation jobs.

      *****************************************************
      *******        PART  VI -- Using TROFF        *******
      *****************************************************

     In this last section, we describe a few tricks for using "vi"
to prepare  "troff" source  code.   If you  do not  have a "troff"
system for your machine, this will not be useful to you.

     For  those  who  do  have  "troff",  we  will assume that you
already know a few of the basics about  using the  text formatter.
This is  a guide  to "vi",  and we don't have the space to scratch
the surface of "troff".  However, we can give you a few tips.

     In "edit.v", we described the  "{"  and  "}"  cursor movement
commands.   "{" goes  back to  the start  of the paragraph and "}"
goes forward to the start of the next  paragraph.   These commands
recognize the "troff" macros and directives

          .IP  .LP  .PP  .QP  .P   .LI  .pp  .lp  .ip  .bp

as ones  that can  begin paragraphs.   Thus  the two commands will
jump to one of these macros if appropriate.

     The command "]]" jumps to the start of  the next  section.  A
section begins  at a formfeed character (CTRL-L) or open brace "{"
at the beginning of a line, or at one of the "troff" macros

          .SH  .NH  .H   .HU

Similarly, the command "[[" jumps  backwards  to  the  start  of a
section.

     In "troff"  input, every sentence should start on a new line.
There should be no blank lines in the input -- if you  want spaces
in the output, use appropriate "troff" commands.

     It may be a good idea to specify the option

                 :set beautify

This gets  rid of all control characters in "vi" input, except for
the formfeed (CTRL-L) and  the  horizontal  tab  (CTRL-I).   Other
control characters  are usually unsuitable in "troff" input, since
they result in odd output.  If you really do want to add a control
character to  a file once the "beautify" option has been set, type
CTRL-V and then the special character you want.

     If you want to format your "troff" input while you  are still
in "vi", you can use

                 :!troff file

where "file"  is the name of the file.  If there are any errors in
the  input,  you  can  let  "troff"  report  them,  then  fix them
immediately because you are still in "vi".  Good luck!

