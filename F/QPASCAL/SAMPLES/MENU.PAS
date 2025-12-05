{ ================================ MENU ===================================

  This unit contains routines that put menus on the screen and handle keyboard
  input.  To use it, specify Menu in the USES section of your program.

  The following routines are public:

   put_menu  -  Puts a menu on screen and reads input for it
   box       -  Puts a box on screen (fill it yourself)
   get_key   -  Gets ASCII or function key

  The following structures are defined:

   menu_t    - Defines menu colors, box type, and centering
   item_list - Defines text of menu item and index of highlight character

  The global variable "menu_attrib" has type menu_t.  Change this variable to
  change menu appearance. The default works for color or B&W.  You can
  override the default value by defining your own menu_t variable and assiging
  it to menu_attrib, or you can modify specific fields at run time.  For example,
  you could use a different attribute for color than for black and white.
}

UNIT menu;

{ =================================================================== }
			INTERFACE
{ =================================================================== }


USES
	Crt, Dos;

CONST
	UpArrow   = $148;  DownArrow  = $150;
	enter     = $0D;   KeypadEnter = $1E0;
	max_items = 10;
	max_item_len = 20;

TYPE
	wait_action = ( no_wait, wait, clear_wait );

	menu_t = RECORD
		fg_border, fg_normal, fg_select, fg_normhilite, fg_selhilite : Integer;
		bg_border : LongInt;
		bg_normal, bg_select, bg_normhilite, bg_selhilite : Integer;
		centered : Boolean;
		ch_nw, ch_ne, ch_se, ch_sw, ch_ns, ch_ew : Char;
		END;

	item_list = RECORD
		hilite : Integer;
		item : STRING[max_item_len];
		END;

	item_array_t = ARRAY[0..max_items] OF item_list;

CONST
	menu_attrib : menu_t = (
		  fg_border : Black;
		  fg_normal : Black;
		  fg_select : LightGray ;
		  fg_normhilite : White ;
		  fg_selhilite: White ;
		  bg_border : LightGray ;
		  bg_normal : LightGray ;
		  bg_select : Black ;
		  bg_normhilite : LightGray ;
		  bg_selhilite : Black ;
		  centered :  True ;
		  ch_nw :  'Ú';
		  ch_ne :  '¿';
		  ch_se :  'Ù';
		  ch_sw :  'À';
		  ch_ns :  '³';
		  ch_ew :  'Ä'
		  ) ;

PROCEDURE box( y, x, rowLast, colLast : Byte );
FUNCTION  get_key( fwait : wait_action ): Word;
PROCEDURE put_menu(     row, col: Byte; item_array : item_array_t;
					VAR current_selection : Integer );

{================================================================}
					   IMPLEMENTATION
{================================================================}
CONST
	previous : Integer = 0;

VAR
	xpos, ypos : Byte;    { saved x and y positions }

{======================= toggle_cursor ============================
 Changes the state of cursor visibility.  Assumes video page 0.

 Params: none

 Returns: TRUE if cursor is visible
}
FUNCTION toggle_cursor : Boolean;
	VAR
		r : Registers;
	BEGIN
	   r.AH := 3;  { Read Cursor function }
	   r.BH := 0;  { Assume page 0 }
	   Intr( 16, r );
	   r.AH := 1;
	   r.CH := r.CH XOR $20; { Toggle visible bit }
	   toggle_cursor := ( r.CH AND $20 ) = 0;
	   Intr( 16, r );
   END; { toggle_cursor }

{=========================== fill_str ==============================
  Fills a string with a specified number of identical characters.

  Params:   st - string to fill
			c  - character to fill with
			n  - number of bytes to fill
}

PROCEDURE fill_str( VAR st : CSTRING; c : Char; n : Byte );

BEGIN
	FillChar( st, n, c );
	st[n + 1] := #0;
END; { fill_str }

{=========================== box ===================================

 Draws menu box, filling interior with blanks of the border color.

	Params:  row and col - upper left of box
			 rowLast and collast - height and width

	Uses: menu_attrib
}

PROCEDURE box( y, x, rowLast, colLast : Byte );

VAR
	i, j  : Byte;
	chars : CSTRING;

BEGIN

	GotoXY( x, y );
	TextColor( menu_attrib.fg_border );
	TextBackground( menu_attrib.bg_border );

	{ Draw top of box. }
	fill_str( chars, menu_attrib.ch_ew, colLast );
	chars := menu_attrib.ch_nw + chars + menu_attrib.ch_ne;
	Write( chars );

	{ Draw sides and center of box. }
	fill_str( chars, ' ', colLast );
	chars := menu_attrib.ch_ns + chars + menu_attrib.ch_ns;
	FOR i := 1 TO rowLast DO
		BEGIN
		GotoXY ( x, y + i );
		Write ( chars );
		END;

	{ Draw bottom of box. }
	GotoXY( x, y + rowLast + 1 );
	fill_str( chars, menu_attrib.ch_ew, colLast );
	chars := menu_attrib.ch_sw + chars + menu_attrib.ch_se;
	Write( chars );

END; { box }

{========================== itemize ===============================
   Displays one selection (item) of a menu.  This procedure is
   used internally by put_menu.

   Params:  row and col - top left of menu
			cur - flag set if item is current selection
			itm - structure containing item text and index of highlight
			n_blanks - length of a line

   Uses:  menu_attrib
}

PROCEDURE itemize( y, x     : Byte;
				   selected : Boolean;
				   itm      : item_list;
				   n_blanks : Byte );

VAR
	i, j : Byte;
	chars : CSTRING;

BEGIN

	{ Set text position and color. }
	GotoXY( x, y );
	IF selected THEN
		BEGIN
		TextColor( menu_attrib.fg_select );
		TextBackground( menu_attrib.bg_select);
		END
	ELSE
		BEGIN
		TextColor( menu_attrib.fg_normal );
		TextBackground( menu_attrib.bg_normal );
		END;  { IF }

	{ Display item and fill blanks. }
	fill_str( chars, ' ', n_blanks - Length( itm.item ) );
	Insert( itm.item, chars, 2 );
	Write( chars );

	{ Set position and color of highlight character, then display it. }
	i := itm.hilite;
	IF  selected THEN
		BEGIN
		TextColor( menu_attrib.fg_selhilite );
		TextBackground( menu_attrib.bg_selhilite );
		END
	ELSE
		BEGIN
		TextColor( menu_attrib.fg_normhilite );
		TextBackground( menu_attrib.bg_normhilite );
		END;

	GotoXY( x + i, y );
	Write( itm.item[i] );
END;  { itemize }

{========================== get_key ============================
  Gets a key from the keyboard.  This routine distinguishes
   between ASCII keys and function or control keys with different shift
   states.  It also accepts a flag to return immediately if no key is
   available.

   Params:
		fwait       Code to indicate how to handle keyboard buffer:
			no_wait     Return 0 if no key in buffer, else return key.
			wait        Return first key if available, else wait for key.
			clear_wait  Throw away any key in buffer and wait for new key.

   Return: One of the following:

	Keytype                                High Byte    Low Byte
	-------                                ---------    --------
	No key available (only with NO_WAIT)       0           0
	ASCII value                                0        ASCII code
	Unshifted function or keypad               1        scan code
	Shifted function or keypad                 2        scan code
	CTRL function or keypad                    3        scan code
	ALT function or keypad                     4        scan code

  Note:   get_key cannot return codes for keys not recognized by BIOS
		  int 16, such as the CTRL-UP or the 5 key on the numeric keypad.

}

FUNCTION get_key ( fwait : wait_action ) : Word;
VAR
	regs : Registers;
CONST
	KeyBoard = $16;
	KRead    : Word = $0;
	KCheck   : Word = $1;
	KShift   : Word = $2;

BEGIN

	{ If bit 8 of the byte at 0:496 is set, then this machine has
	  a new keyboard.  Use it to get F11, F12, etc.
	}
	IF ((Mem[0:$496] AND $10) <> 0) THEN
		BEGIN
		KRead := $10;
		KCheck := $11;
		KShift := $12;
		END;

	WITH regs DO
		BEGIN
		{ If CLEAR_WAIT, drain the keyboard buffer. }
		IF (fwait = clear_wait) THEN
			REPEAT
				AH := KCheck;
				Intr( KeyBoard, regs );
			UNTIL ((Flags AND $40) = 0);   { Until Zero flag is set }

		{ If NO_WAIT, return 0 if there is no key ready. }
		IF (fwait = no_wait) THEN
			BEGIN
			AH := KCheck;
			Intr( KeyBoard, regs );
			IF ((Flags AND $40) <> 0) THEN
				BEGIN
				get_key := 0;
				Exit;
				END;
			END;

		{ Get key code. }
		AH := KRead;
		Intr( KeyBoard, regs );
		BH := AH;

		{ If low byte is not zero, it's an ASCII key. Check scan code to see
		  if it's on the numeric keypad. If not, clear high byte and return.
		}
		IF (AL <> 0) THEN
			IF (AH < 69) THEN
				BEGIN
				AH := 0;
				get_key := AX;
				Exit;
				END;

		{ For function keys and numeric keypad, put scan code in low byte
		  and shift state codes in high byte.
		}
		AH := KShift;
		Intr( KeyBoard, regs );
		CASE (AL AND $0F) OF
			0 :         AH := 1;        { None  (1)   }
			1, 2, 3:    AH := 2;        { Shift (2)   }
			4:          AH := 3;        { Control (3) }
			8:          AH := 4;        { Alt (4)     }
			ELSE        AH := 1;        { Other (1)   }
			END; { CASE }
		AL := BH;
		get_key := AX;
		END; { WITH }
END; { get_key }

{============================= put_menu ============================
  Puts menu on screen and reads menu input from keyboard.
  When the user presses ENTER or a highlighted hot key, put_menu
  returns the index of the selected menu item.

  Params:
  row and col - If "centered" attribute of "menu_attrib" is true,
				center row and column of menu; otherwise, top left of
				menu
  item_array - array of structure containing the text of each item
			   and the index of the highlighted hot key.
  current_selection - index of the current selection -- pass 0 for first
			   item, or maintain a static value

  Return:  The index of the selected item.

  Uses: menu_attrib
}
PROCEDURE put_menu (     row, col : Byte;
						 item_array : item_array_t;
					 VAR current_selection : Integer );

VAR
	n_items, longest_item : Byte;
	hilite_chars : ARRAY [0..max_items] OF Char;
	orig_mode : Word;
	orig_attr : Byte;
	old_cursor : Boolean;
	ukey : Word;
	ch_pos : Byte;
	i : Integer;

BEGIN

	{ Save screen information. }
	orig_mode := LastMode;
	orig_attr := TextAttr;
	xpos := WhereX;
	ypos := WhereY;
	old_cursor := True;
	IF toggle_cursor THEN old_cursor := toggle_cursor;

	{ Count items, find longest, and put count of each in array.  Also,
	  put the highlighted character from each in a string.
	}
	n_items := 0;
	longest_item := 0;
	WHILE (item_array[n_items].item <> '') DO
		WITH item_array[n_items] DO
			BEGIN
			IF (Length( item ) > longest_item) THEN
				longest_item := Length( item );
			hilite_chars[n_items] :=
				UpCase( item[hilite] );
			Inc( n_items );
			END; { WITH statement }

	{ Longest item sets width of menu box.  Add two for esthetics. }
	longest_item := longest_item + 2;

	{  Adjust if centered, set row and col to upper left corner
	   of menu, and draw menu box.
	}
	IF menu_attrib.centered THEN
	   BEGIN
		 row := row - (n_items DIV 2);
		 col := col - (longest_item  DIV 2);
	   END;
	box( row, col, n_items, longest_item );

	{ Insert the items in the menu. }
	row := row + 1;
	col := col + 1;
	FOR i := 0 TO n_items - 1 DO
		IF (i = current_selection) THEN
			itemize( row + i, col, True, item_array[i],
					 longest_item )
		ELSE
			itemize( row + i, col, False, item_array[i],
					 longest_item );

	WHILE True DO
		BEGIN
		{ Wait until a ukey is pressed, then evaluate it. }
		ukey := get_key( wait );

		CASE  ukey  OF
			UpArrow : BEGIN
				previous := current_selection;
				IF ( current_selection > 0) THEN
					BEGIN
					current_selection := current_selection - 1;
					current_selection := current_selection MOD n_items;
					END
				ELSE
					current_selection := n_items - 1;
				END; { case UpArrow }

			DownArrow : BEGIN
				previous := current_selection;
				IF ( current_selection < n_items) THEN
					BEGIN
					current_selection := current_selection + 1;
					current_selection := current_selection MOD n_items;
					END
				ELSE
					current_selection := 0;
				END;  { case DownArrow }
			enter, KeypadEnter :
				BEGIN
				TextMode( orig_mode );
				TextAttr := orig_attr;
				GotoXY( xpos, ypos );
				IF old_cursor THEN old_cursor := toggle_cursor;
				Exit;
				END;  { case Enter, KeypadEnter }
			ELSE BEGIN
				IF (Hi( ukey ) = 0) THEN
					BEGIN
					ch_pos := Pos( UpCase( Chr(Lo( ukey ) ) ), hilite_chars );
					IF (ch_pos <> 0) THEN
						BEGIN
						previous := current_selection;
						current_selection := ch_pos - 1; { Menu array starts with 0 }
						Exit;
						END;
					END;
				END;  { ELSE clause of CASE statement }
	   END; { CASE statement }

	   { Redisplay current and previously selected items. }
	   itemize( row + current_selection, col, True, item_array[current_selection],
				longest_item );
	   IF (current_selection <> previous) AND (previous < n_items) THEN
		  itemize( row + previous, col, False, item_array[previous],
				   longest_item );
	   END; { WHILE true }

END;   { put_menu }

END. { unit menu }









