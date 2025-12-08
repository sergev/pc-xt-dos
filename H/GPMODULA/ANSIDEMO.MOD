MODULE ANSIDemo;
(*
   Demonstrate some of the capabilities of the ANSISYS module.
*)

FROM ANSISYS IMPORT
        Attribute, SetAttribute, Mode, SetMode, ResetMode,
        ClearScreen, GotoRowCol, GetRowCol,
        Up, Down, Right, Left, Save, Restore, EraseToEOL;
FROM InOut IMPORT Read, Write, WriteString, WriteCard, WriteLn;
FROM Terminal IMPORT GetKeyStroke;
FROM Ascii IMPORT nul;

VAR
  ch : CHAR;
  row, col : CARDINAL;

BEGIN
  WriteString ("This program demonstrates some facilities of the ANSISYS module.");
  WriteLn;
  WriteLn; WriteLn;
  WriteString ("It first displays some atttribute settings and a menu,");
  WriteLn;
  WriteString ("and allows you to experiment with cursor positioning, etc.");
  WriteLn;
  WriteString ("If the menu is not readable, you may need to change the");
  WriteLn;
  WriteString ("colours currently used (magenta on green - revolting! - at");
  WriteLn;
  WriteString ("least one VGA card produces grey on grey on a monochrome monitor).");
  WriteLn; WriteLn;
  WriteString ('When you terminate this stage with "q", it demonstrates some other');
  WriteLn;
  WriteString ('modes; each stage will direct you to terminate with either "q" or');
  WriteLn;
  WriteString ("enter or any key.");
  WriteLn; WriteLn;
  WriteString ("Note that the display is left in 80x25 mode, yellow on blue;");
  WriteLn;
  WriteString ("you may also want to change this, or use MODE afterwards.");
  WriteLn; WriteLn;
  WriteString ("The program can then be used as a basis for your own code.");
  WriteLn; WriteLn;
  WriteString ("Any key to begin: ");
  GetKeyStroke (ch);

  (* Start with clearing, positioning, attributes *)
  ClearScreen;
  SetAttribute (allOff);
  WriteString ("Demonstration of some capabilities of the ANSISYS module.");
  GotoRowCol (2,1);
  WriteString ("These lines are in 'all attributes off' mode = white on black");

(* These demonstrate out-of-range coordinates:
  GotoRowCol (0,5); Write ('A');        (* => 1,5 *)
  GotoRowCol (5,9); Write ('X');        (* Cursor now at 5,10 => *)
  GotoRowCol (40,5); Write ('B');       (* Goto ignored => 5,10  *)
  GotoRowCol (5,0); Write ('C');        (* => 5,1 *)
  GotoRowCol (5,100); Write ('D');      (* => 5,80 *)
  GotoRowCol (0,0); Write ('E');        (* => 1,1 *)
  GotoRowCol (0,100); Write ('F');      (* => 1,80 *)
  GotoRowCol (7,9); Write ('Y');        (* Cursor now at 7,10 =>   *)
  GotoRowCol (50,0); Write ('G');       (* Goto ignored => 7,10 => *)
  GotoRowCol (50,100); Write ('H');     (* Goto ignored => 7,11    *)
*)

  GotoRowCol (8,40);
  SetAttribute (bold); WriteString ("Bold"); SetAttribute (allOff);

  GotoRowCol (8,60);
  SetAttribute (faint); WriteString ("Faint"); SetAttribute (allOff);

  GotoRowCol (10,40);
  SetAttribute (italic); WriteString ("Italic"); SetAttribute (allOff);

  GotoRowCol (10,60);
  SetAttribute (underscore); WriteString ("Underscore (MDA)");
  SetAttribute (allOff);

  GotoRowCol (12,40);
  SetAttribute (blink); WriteString ("Blink"); SetAttribute (allOff);

  GotoRowCol (12,60);
  SetAttribute (rapidBlink); WriteString ("Rapid blink"); SetAttribute (allOff);

  GotoRowCol (14,40);
  SetAttribute (reverse); WriteString ("Reverse - black on white");
  SetAttribute (allOff);

  GotoRowCol (14,60);
  SetAttribute (concealed); WriteString ("Concealed!"); SetAttribute (allOff);

  GotoRowCol (16,40);
  SetAttribute (bold); SetAttribute (yellowFG); SetAttribute (blueBG);
  SetAttribute (blink);
  WriteString ("Blink in yellow on blue");
  SetAttribute (allOff);

  (* Now interactive stuff *)
  SetAttribute (magentaFG); SetAttribute (greenBG);
  GotoRowCol (20,0);
  WriteString ("Now interactive screen update in magenta on green:");
  GotoRowCol (21,0);
  WriteString ("arrows for left/right/up/down, or home;  ");
  WriteString ("s/r for save/restore;");
  GotoRowCol (22,0);
  WriteString ("c/e for clear screen/erase to eol; n/w for nowrap/wrap; ");
  GotoRowCol (23,0);
  WriteString ("? to read and display cursor position; q to quit;");
  GotoRowCol (24,0);
  WriteString ("all other keys just echo, preceded by ! if extended.");
  LOOP
    GetKeyStroke (ch);
    CASE ch OF
    | nul : (* Extended ASCII sequence - act on second byte *)
            GetKeyStroke (ch);
            CASE ch OF
            | 107C : GotoRowCol (1,1);          (* Home *)
            | 110C : Up (1);                    (* Up arrow *)
            | 113C : Left (1);                  (* Left arrow *)
            | 115C : Right (1);                 (* Right arrow *)
            | 120C : Down (1);                  (* Down arrow *)
            ELSE Write ('!'); Write (ch);       
            END;
    | '?' : GetRowCol (row, col);
            WriteCard (row,1);
            Write (",");
            WriteCard (col,1);
    | 's' : Save;
    | 'r' : Restore;
    | 'c' : ClearScreen;
    | 'e' : EraseToEOL;
    | 'n' : ResetMode (cursorWrap);
    | 'w' : SetMode (cursorWrap)
    | 'q' : EXIT;
    ELSE Write (ch); (* just echo *)
    END;
  END;

  (* Finally, other modes *)
  SetMode (textColour40x25); (* default clear screen *)
  SetAttribute (bold);
  SetAttribute (cyanFG);
  SetAttribute (greenBG);
  WriteString ("Should now be in");
  GotoRowCol (2,0);
  WriteString ("text colour 40 x 25 mode");
  GotoRowCol (20,20);
  WriteString ("<- row 20, column 20;");
  GotoRowCol (21,20);
  WriteString ("any key to continue");
  GetKeyStroke (ch);

  (* Show that ResetMode is equivalent to SetMode *)
  ResetMode (textMono80x25); (* default clear screen *)
  WriteString ("ResetMode to text mono 80x25");
  GotoRowCol (25,1);
  WriteString ("any key to continue");
  GetKeyStroke (ch);

  (* Show that text I/O is still possible in graphics mode *)
  ResetMode (graphicsMono640x200); (* default clear screen *)
  WriteString ("Graphics 640x200");
  GotoRowCol (10,10);
  WriteString ("<- 10,10 (i.e. equivalent to text 80x25 - 10,10 -> 80,80)");
  GotoRowCol (20,1);
  WriteString ("anything, then <enter> to continue: ");
  Read (ch);

  (* This one will be ignored on CGA & EGA, but will work on VGA *)
  ResetMode (graphicsColour320x200x256); (* default clear screen *)
  WriteString ("Graphics 320x200x256? ");
  GotoRowCol (10,10);
  WriteString ("<- 10,10 (i.e. equivalent to text 40x25 - 10,10 -> 80,80)");
  GotoRowCol (20,1);
  WriteString ("... anything, then 'q' to continue: ");
  REPEAT
    GetKeyStroke (ch);
    Write (ch);
  UNTIL ch = 'q';

  (* Restore 'normal' modes *)
  SetMode (cursorWrap);
  SetMode (textColour80x25);
  SetAttribute (bold);
  SetAttribute (yellowFG);
  SetAttribute (blueBG);

END ANSIDemo.
