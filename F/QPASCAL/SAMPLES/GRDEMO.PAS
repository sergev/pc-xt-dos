PROGRAM grdemo;
{ GRDEMO.PAS demonstrates the QuickPascal graphics library.
  It uses two additional units: menu and turtle (for turtle
  graphics).
}

USES
	MSGraph, Crt, menu, turtle;

CONST

	{ Contents of main menu.  The hilite field gives the
	  position of the hot key.
	}
	main_menu : item_array_t =
		( ( hilite : 1; item :  'Quit' ),
		  ( hilite : 1; item :  'Circles' ),
		  ( hilite : 1; item :  'Rotating Sphere' ),
		  ( hilite : 1; item :  'Tunnel' ),
		  ( hilite : 1; item :  'Spiral' ),
		  ( hilite : 1; item :  'Inverted Spiral' ),
		  ( hilite : 1; item :  'Bug' ),
		  ( hilite : 1; item :  'Adjust Window' ),
		  ( hilite : 1; item :  'Mode Change' ),
		  ( hilite : 1; item :  '' ) ,
		  ( hilite : 1; item :  '' )
		);

	mode_msg = 'Cannot set graphics mode.';

	{ Constants for main menu choices }
	do_quit = 0;
	do_circles = 1;
	do_sphere = 2;
	do_tunnel = 3;
	do_spiral = 4;
	do_inverted_spiral = 5;
	do_bug = 6;
	do_adjust = 7;
	do_change_mode = 8;

TYPE
	mode_array_t = ARRAY[0..14] OF Integer;

VAR
	current_main : Integer;     { Current choice from main menu }
	current_mode : Integer;     { Current choice from modes menu }
	modes_array  : mode_array_t; { Indexed by current_mode }
	modes_menu   : item_array_t; { Menu of graphics modes }
	bool_val, color : Boolean;
	vc : _VideoConfig;
	row_mid, col_mid : Byte;     { Middle of the screen }
	mode : Integer;              { Mode in effect }
	return_code : Integer;
	ch : Char;
	anykey : Word;

{ ============================ get_random =============================
  Get_random returns a random integer in the range specified by
  its parameters.
}

FUNCTION get_random( min, max: Integer ) : Integer;

	BEGIN
		get_random := Random( max - min ) + min;
	END;   { function get_random }

{ ============================= adjust  =================================
  Changes the aspect ratio, window size, and window location according
  to user's input.
 }

PROCEDURE adjust;

VAR
	left, right, top, bottom : Integer;
	i         : Integer;
	fmt, tmp  : CSTRING;
	key       : Word;
	vc        : _VideoConfig;

CONST
	radius_xy : Real = 400.0;
	win_inc = 4;
	u_up = $0148;  { Up arrow }
	u_dn = $0150;  { Down arrow }
	u_lt = $014B;  { Left arrow }
	u_rt = $014d;  { Right arrow }
	s_up = $0248;  { SHIFT + up arrow }
	s_dn = $0250;  { SHIFT + down arrow }
	s_lt = $024B;  { SHIFT + left arrow }
	s_rt = $024d;  { SHIFT + right arrow }
	n_plus = $014E;  { Gray plus key }
	n_minus = $014A; { Gray minus key }
	enter  = 13;     { Enter key }
	keypadenter = $1E0;  { Gray enter key }

BEGIN
	_GetVideoConfig( vc );
	WHILE True DO
		BEGIN
		_SetTextPosition( 1, 2 );
		_OutText(' Grey PLUS and MINUS Adjust aspect' );
		_SetTextPosition( 2, 2 );
		_OutText(' Cursor keys         Size window' );
		_SetTextPosition( 3, 2 );
		_OutText(' SHIFT cursor keys   Move window' );
		_SetTextPosition( 4, 2 );
		_OutText(' ENTER               Finished' );

		Str( tratioyx:5:2, tmp );
		fmt := 'ratio = ' + tmp + '   xMax = ';
		Str( tmaxx:5:2, tmp );
		fmt := fmt + tmp + '   yMax = ';
		Str( tmaxy:5:2, tmp );
		fmt := fmt + tmp;
		_SetTextPosition( 6, 2 );
		_OutText( fmt );

		{ Draw border and circle. }
		rectangle( 2 * tmaxx,  2 * tmaxy );
		pendown( False );
		moveto( 75.0, 0.0 );
		pendown( True );
		circle( radius_xy );
		FOR i := 1 TO 4 DO
			BEGIN
			pendown( True );
			Move( radius_xy );
			turn( 180 );
			pendown( False );
			Move( radius_xy );
			turn( 90 );
			END;

		{ Read user input and adjust values accordingly. }
		gettwindow( left, top, right, bottom );
		key := get_key( CLEAR_WAIT );
		CASE key OF
			n_minus:
				tratioyx := (tmaxx - (win_inc * tunit)) / tmaxy;
			n_plus:
				tratioyx := (tmaxx + (win_inc * tunit)) / tmaxy;
			u_rt:
				BEGIN
				IF (left < (vc.NumXPixels DIV 3) ) THEN
					left := left + win_inc;
				IF( right > (vc.NumXPixels - (vc.NumXPixels DIV 3)) ) THEN
					right := right - win_inc;
				END;
			u_lt:
				BEGIN
				IF( left <> 0 ) THEN
					left := left - win_inc;
				IF( right < vc.NumXPixels ) THEN
					right := right + win_inc;
				END;
			u_dn:
				BEGIN
				IF (top < (vc.NumYPixels DIV 3) ) THEN
					top := top + win_inc;
				IF (bottom > (vc.NumYPixels - (vc.NumYPixels DIV 3)) ) THEN
					bottom := bottom - win_inc;
				END;
			u_up:
				BEGIN
				IF( top <> 0 ) THEN
					top := top - win_inc;
				IF( bottom < vc.NumYPixels ) THEN
					bottom := bottom + win_inc;
				END;
			s_lt:
				IF( left <> 0 ) THEN
					BEGIN
					left := left - win_inc;
					right := right - win_inc;
					END;
			s_rt:
				IF( right < vc.NumXPixels ) THEN
					BEGIN
					left := left + win_inc;
					right := right +  win_inc;
					END;
			s_up:
				IF( top <> 0 ) THEN
					BEGIN
					top := top - win_inc;
					bottom := bottom - win_inc;
					END;
			s_dn:
				IF( bottom < vc.NumYPixels ) THEN
					BEGIN
					top := top + win_inc;
					bottom := bottom + win_inc;
					END;

			enter, keypadenter:
				Exit;

			ELSE
				home;
			END; { CASE }
		_ClearScreen( _GClearScreen );
		settwindow( left, top, right, bottom );
		home;
		END; { WHILE }
END; { adjust }

{ =========================== circles ==============================
  Draws circles of varying sizes and colors on screen in a
  round pattern.

  Params: None
}
PROCEDURE circles;
VAR
	x, y, radius_xy  : Double;
	fill_flag, pen_flag : Boolean;

BEGIN
	{ Initialize and save pen and fill flags. }
	IF (tcolorindexes <= 4) THEN
		fillon( False )
	ELSE
		fillon( True );
	fill_flag := fillstate;
	pen_flag := penstate;
	pendown( False );

	WHILE True DO
		BEGIN

		{ Draw circles. }
		radius_xy := 10.0;
		WHILE (radius_xy <= 130.0) DO
			BEGIN
			x := (tmaxx - 30) * ArcTan( Sin( radius_xy / Pi ) );
			y := (tmaxy - 30) * ArcTan( Cos( radius_xy / Pi ) );
			moveto( x, y );
			pencolor( nextcolorindex( default ) );
			circle( radius_xy );
			IF (get_key( no_wait ) <> 0) THEN
				BEGIN
				pendown( pen_flag );
				fillon( fill_flag );
				Exit;
				END;
			radius_xy := radius_xy + 1.0;
			END; { WHILE }

		{ For palette modes (except 256 color), start over. }
		IF (tcolorvalues = 64) OR (tcolorvalues = 16) THEN
			BEGIN
			_ClearScreen( _GClearScreen );
			fillon( False );
			moveto( 0.0, 0.0 );
			pencolor( White );
			rectangle( 2 * tmaxx, 2 * tmaxy );
			fillon( fill_flag );
			nextcolorvalue( default );
			END;
		END; { WHILE }
END;  { circles }


{ =========================== sphere ==============================
  Draws and fills slices of a sphere. Rotates colors in EGA+ modes
  with more than 4 color indexes.

  Params: None
}
PROCEDURE sphere;
VAR
	xcur, xsize, ysize, xinc : Double;
	cvi, ci, c, border_color, ret : Integer;
	fill_flag : Boolean;

BEGIN
	cvi := 0; ci := 0; c := 0;
	xsize := tmaxy * 0.9 * 2;
	ysize := xsize;
	fill_flag := fillstate;
	fillon( False );
	ret := nextcolorindex( 0 );
	xinc := xsize / 14;
	border_color := getpencolor;
	bordercolor( border_color );

	{ Draw slices. }
	xcur := xinc;
	WHILE (xcur <= xsize) DO
		BEGIN
		ellipse( xcur, ysize );
		xcur := xcur + (xinc * 2);
		END;
	fillon( True );
	pendown( False );
	turn( 90 );
	xsize := xsize / 2;
	moveto( xsize - xinc, 0.0 );

	nextcolorvalue( limited );

	{ Fill slices. }
	WHILE tcurx >= (-xsize + xinc) DO
		BEGIN
		pencolor( nextcolorindex( default ) );
		paint;
		Move( -xinc );
		END;

	WHILE ( get_key( no_wait ) = 0) DO
		nextcolorvalue( limited );

	pendown( True );
	fillon( fill_flag );
END; { sphere }

{ =========================== polygons ==============================
  Draws polygons (starting with triangle) of increasing size
  by incrementing the number of sides without changing the
  length of sides. Make sure pen is down.

  Params: None

  Return: 1 for user interrupt, 0 for edge of screen encountered

}
FUNCTION polygons : Boolean;
VAR
	sides, atrib : Integer;
	dxy : Double;

BEGIN
	sides := 3;
	atrib := 1;
	dxy := tunit;
	WHILE True DO
		BEGIN
		pencolor( nextcolorindex( default ) );
		Inc( sides );
		dxy := dxy + 1.5;
		poly( sides, dxy );
		IF NOT turtlestat THEN
			BEGIN
			polygons := False;
			Exit;
			END;
		IF ( get_key( no_wait ) <> 0) THEN
			BEGIN
			polygons := True;
			Exit;
			END;
		END;
END;  { polygons }


{ =========================== spiral ==============================
  Draws a spiral by incrementing the length of each side
  of a rotating figure.

  Params: ang - determines tightness
		  xyInc - determines size of sides

  Return: 1 for user interrupt, 0 for edge of screen encountered

}
FUNCTION spiral( ang : Integer; xyinc : Double ) : Boolean;
VAR
	xy : Double;

BEGIN
	xy := tunit;

	WHILE True DO
		BEGIN
		pencolor( nextcolorindex( default ) );
		xy := xy + xyinc;
		Move( xy );
		IF NOT turtlestat THEN
			BEGIN
			spiral := False;
			Exit;
			END;
		turn( ang );
		IF (get_key( no_wait ) <> 0) THEN
			BEGIN
			spiral := True;
			Exit;
			END;
		END;
END; { spiral }

{ =========================== inspiral ==============================
  Draws an inverted spiral by increasing each angle of a rotating
  figure while keeping the length of sides constant.

  Params: xy - determines size
		  ang - initial angle determines shape
		  ang_inc - determines tightness and shape

  Return: 1 for user interrupt, 0 for edge of screen encountered
}
FUNCTION inspiral( xy : Double; ang, ang_inc : Integer ) : Boolean;
BEGIN
	WHILE True DO
		BEGIN
		pencolor( nextcolorindex( default ) );
		Move( xy );
		IF NOT turtlestat THEN
			BEGIN
			inspiral := False;
			Exit;
			END;
		ang := ang + ang_inc;
		turn( ang );
		IF (get_key( no_wait ) <> 0) THEN
			BEGIN
			inspiral := True;
			Exit;
			END;
		END;
END; { inspiral }

{ =========================== bug ==================================
  Draws a winged bug on the screen, then moves it around in a random
  pattern.

  Params: None
}

PROCEDURE bug;
TYPE
	bigbuf_t   = ARRAY[1..65520] OF Byte;

CONST
	top_wing : _FillMask = ( $81, $3c, $c3, $66, $66, $0f, $f0, $18 );
	uBotWing : _FillMask = ( $66, $0f, $f0, $18, $81, $3c, $c3, $66 );
	Blank    : _FillMask = ( $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff );

VAR
	buffer : ^Byte;
	imsize : LongInt;
	stat   : Integer;

BEGIN
	{ Draw bug. }
	pendown( False );
	fillon( True );
	Move( 40.0 );               { Draw and fill front wings. }
	turn( 90 );
	Move( 80.0 );
	pencolor( 1 );
	_SetFillMask( top_wing );
	ellipse( 172.0, 70.0 );
	turn( 180 );
	Move( 160.0 );
	ellipse( 172.0, 70.0 );
	turn(-90 );
	moveto( 0.0, 0.0 );
	Move( 25.0 );               { Draw and fill back wings. }
	turn( 90 );
	Move( 70.0 );
	pencolor( 2 );
	_SetFillMask( uBotWing );
	ellipse( 150.0, 70.0 );
	turn( 180 );
	Move( 140.0 );
	ellipse( 150.0, 70.0 );
	turn( -90 );
	moveto( 0.0, 0.0 );
	_SetFillMask( Blank );      { Draw body. }
	pencolor( 3 );
	bordercolor( 3 );
	ellipse( 52.0, 220.0 );
	pencolor( 1 );              { Drill eyes. }
	bordercolor( 1 );
	fillon( False );
	Move( 90.0 );
	turn( 90 );
	Move( 22.0 );
	circle( 20.0 );
	pencolor( 0 );
	paint;
	pencolor( 1 );
	turn( 180 );
	Move( 44.0 );
	circle( 20.0 );
	pencolor( 0 );
	paint;

	{ Move into position - top-right of image. }
	moveto( 0.0, 0.0 );
	turnto( 0 );
	Move( 120.0 );
	turn( -90 );
	Move( 175.0 );
	turn( 90 );

	{ Size image and allocate memory for it. }
	imsize := imagesize( 350.0, 240.0 );
	GetMem( buffer, Word( imsize ) );
	getimage( 350.0, 240.0, buffer^ );
	stat := _GrStatus;

	{ Move randomly, adjusting at edges. }
	WHILE (get_key( no_wait ) = 0) DO
		BEGIN
		IF tcurx <= (-tmaxx + 15.0) THEN
			turnto( 90 )
		ELSE IF tcury <= (-tmaxy + 15.0) THEN
			turnto( 180 )
		ELSE IF tcurx >= (tmaxx - 365.0) THEN
			turnto( 270 )
		ELSE IF tcury >= (tmaxy - 255.0) THEN
			turnto( 0 )
		ELSE
			turn( get_random( -20, 20 ) );
		Move( 3.0 );
		putimage( buffer^, _GPSet );
		END;
	FreeMem( buffer, Word( imsize ) );
END;  { bug }

{ ========================= load_modes ===============================
  Loads an array with menu items representing all graphics modes
  that apply to this video adapter.  Also loads an array with
  the constants for each graphics mode.  The indexes into the
  arrays are equivalent.

  Params:
  adapter - Video adapter
  mm      - Array containing menu items (output)
  ma      - Array containing graphics mode constants (output)
  m       - Preferred initial mode for this adapter (output)

  Returns:
  True if the program supports the video adapter present;
  False otherwise.
}
FUNCTION load_modes(     adapter : Integer;
					 VAR mm   : item_array_t;
					 VAR ma   : mode_array_t;
					 VAR m    : Integer ) : Boolean;

BEGIN

	load_modes := True;
	CASE adapter OF
	_OCGA:   { Enable Olivetti mode. }
		BEGIN
		ma[0] := _OResColor;
		mm[0].hilite := 1;
		mm[0].item := 'ORESCOLOR';
		ma[1] := _MRes4Color;
		mm[1].hilite := 5;
		mm[1].item := 'MRES4COLOR';
		ma[2] := _MResNoColor;
		mm[2].item := 'MRESNOCOLOR';
		mm[2].hilite := 5;
		ma[3] := _HResBW;
		mm[3].item := 'HRESBW';
		mm[3].hilite := 5;
		mm[4].item := '';
		m := _MRes4Color;
		END;
	_CGA:    { Disable EGA modes. }
		BEGIN
		ma[0] := _MRes4Color;
		mm[0].hilite := 5;
		mm[0].item := 'MRES4COLOR';
		ma[1] := _MResNoColor;
		mm[1].item := 'MRESNOCOLOR';
		mm[1].hilite := 5;
		ma[2] := _HResBW;
		mm[2].item := 'HRESBW';
		mm[2].hilite := 5;
		mm[3].item := '';
		m := _MRes4Color;
		END;
	_HGC:
		BEGIN
		ma[0] := _MRes4Color;
		mm[0].hilite := 5;
		mm[0].item := 'MRES4COLOR';
		ma[1] := _MResNoColor;
		mm[1].item := 'MRESNOCOLOR';
		mm[1].hilite := 5;
		ma[2] := _HResBW;
		mm[2].item := 'HRESBW';
		mm[2].hilite := 5;
		ma[3] := _MRes16Color;
		mm[3].item := 'MRES16COLOR';
		mm[3].hilite := 1;
		ma[4] := _HRes16Color;
		mm[4].item := 'HRES16COLOR';
		mm[4].hilite := 1;
		ma[5] := _EResColor;
		mm[5].item := 'ERESCOLOR';
		mm[5].hilite := 1;
		mm[6].item := '';
		m := _HercMono;
		END;
	_OEGA:   { Enable Olivetti modes; disable VGA modes. }
		BEGIN
		ma[0] := _OResColor;
		mm[0].hilite := 1;
		mm[0].item := 'ORESCOLOR';
		ma[1] := _MRes4Color;
		mm[1].hilite := 5;
		mm[1].item := 'MRES4COLOR';
		ma[2] := _MResNoColor;
		mm[2].item := 'MRESNOCOLOR';
		mm[2].hilite := 5;
		ma[3] := _HResBW;
		mm[3].item := 'HRESBW';
		mm[3].hilite := 5;
		ma[4] := _MRes16Color;
		mm[4].item := 'MRES16COLOR';
		mm[4].hilite := 1;
		ma[5] := _HRes16Color;
		mm[5].item := 'HRES16COLOR';
		mm[5].hilite := 1;
		ma[6] := _EResColor;
		mm[6].item := 'ERESCOLOR';
		mm[6].hilite := 1;
		mm[7].item := '';
		IF vc.Memory > 64 THEN m := _EResColor
		ELSE m := _HRes16Color;
		END;
	_EGA:     { Disable VGA modes. }
		BEGIN
		ma[0] := _MRes4Color;
		mm[0].hilite := 5;
		mm[0].item := 'MRES4COLOR';
		ma[1] := _MResNoColor;
		mm[1].item := 'MRESNOCOLOR';
		mm[1].hilite := 5;
		ma[2] := _HResBW;
		mm[2].item := 'HRESBW';
		mm[2].hilite := 5;
		ma[3] := _MRes16Color;
		mm[3].item := 'MRES16COLOR';
		mm[3].hilite := 1;
		ma[4] := _HRes16Color;
		mm[4].item := 'HRES16COLOR';
		mm[4].hilite := 1;
		ma[5] := _EResColor;
		mm[5].item := 'ERESCOLOR';
		mm[5].hilite := 1;
		mm[6].item := '';
		IF (vc.Memory > 64) THEN m := _EResColor
		ELSE m := _HRes16Color;
		END;
	_OVGA:    { Enable Olivetti modes. }
		BEGIN
		ma[0] := _OResColor;
		mm[0].hilite := 1;
		mm[0].item := 'ORESCOLOR';
		ma[1] := _MRes4Color;
		mm[1].hilite := 5;
		mm[1].item := 'MRES4COLOR';
		ma[2] := _MResNoColor;
		mm[2].item := 'MRESNOCOLOR';
		mm[2].hilite := 5;
		ma[3] := _HResBW;
		mm[3].item := 'HRESBW';
		mm[3].hilite := 5;
		ma[4] := _MRes16Color;
		mm[4].item := 'MRES16COLOR';
		mm[4].hilite := 1;
		ma[5] := _HRes16Color;
		mm[5].item := 'HRES16COLOR';
		mm[5].hilite := 1;
		ma[6] := _EResColor;
		mm[6].item := 'ERESCOLOR';
		mm[6].hilite := 1;
		ma[7] := _VRes2Color;
		mm[7].item := 'VRES2COLOR';
		mm[7].hilite := 5;
		ma[8] := _VRes16Color;
		mm[8].item := 'VRES16COLOR';
		mm[8].hilite := 1;
		ma[9] := _MRes256Color;
		mm[9].item := 'MRES256COLOR';
		mm[9].hilite := 2;
		mm[10].item := '';
		m := _VRes16Color;
		END;
	_VGA:
		BEGIN
		ma[0] := _MRes4Color;
		mm[0].hilite := 5;
		mm[0].item := 'MRES4COLOR';
		ma[1] := _MResNoColor;
		mm[1].item := 'MRESNOCOLOR';
		mm[1].hilite := 5;
		ma[2] := _HResBW;
		mm[2].item := 'HRESBW';
		mm[2].hilite := 5;
		ma[3] := _MRes16Color;
		mm[3].item := 'MRES16COLOR';
		mm[3].hilite := 1;
		ma[4] := _HRes16Color;
		mm[4].item := 'HRES16COLOR';
		mm[4].hilite := 1;
		ma[5] := _EResColor;
		mm[5].item := 'ERESCOLOR';
		mm[5].hilite := 1;
		ma[6] := _VRes2Color;
		mm[6].item := 'VRES2COLOR';
		mm[6].hilite := 5;
		ma[7] := _VRes16Color;
		mm[7].item := 'VRES16COLOR';
		mm[7].hilite := 1;
		ma[8] := _MRes256Color;
		mm[8].item := 'MRES256COLOR';
		mm[8].hilite := 2;
		mm[9].item := '';
		m := _VRes16Color;
		END;

	_MCGA:
		BEGIN
		ma[0] := _MRes4Color;
		mm[0].hilite := 5;
		mm[0].item := 'MRES4COLOR';
		ma[1] := _MResNoColor;
		mm[1].item := 'MRESNOCOLOR';
		mm[1].hilite := 5;
		ma[2] := _HResBW;
		mm[2].item := 'HRESBW';
		mm[2].hilite := 5;
		ma[3] := _VRes2Color;
		mm[3].item := 'VRES2COLOR';
		mm[3].hilite := 5;
		ma[4] := _MRes256Color;
		mm[4].item := 'MRES256COLOR';
		mm[4].hilite := 2;
		mm[5].item := '';
		m := _MRes256Color;
		END;
	ELSE
		load_modes := False;
	END; { case }

END; { load_modes }

{ ====================== main program body ============================= }

BEGIN

	{ Disable cursor.  Find out video configuration so we can
	  determine the valid graphics mode for this device.
	}
	bool_val := _DisplayCursor( False );
	_GetVideoConfig( vc );

	row_mid := vc.NumTextRows DIV 2;
	col_mid := vc.NumTextCols DIV 2;

	{ Select best graphics mode and load menu with valid
	  modes for this device.
	}
	IF NOT(load_modes( vc.Adapter, modes_menu, modes_array, Mode )) THEN
		BEGIN
		Writeln( 'No graphics mode available.' );
		Halt( 1 );
		END;

	CASE vc.Mode OF
		_TextBW80, _TextBW40 :
			color := False;
		_TextMono, _HercMono, _EResNoColor :
			BEGIN
			color := False;
			IF Mode <> _HercMono THEN Mode := _EResNoColor;
			main_menu[8].item := '';  { Disable mode change. }
			END;
		ELSE
			color := True;
		END; { CASE }

	{ Initialize random number generator. }
	Randomize;

	{ Initialize main menu and mode selection. }
	current_main := 0;
	current_mode := 0;
	WHILE (Mode <> modes_array[current_mode]) DO
		Inc( current_mode );

	WHILE (True) DO
		BEGIN
		{ Set text mode and optionally clear the screen to blue. }
		return_code := _SetVideoMode( _DefaultMode );
		IF (color) THEN  _SetBkColor( LongInt( Blue ) );
		_ClearScreen( _GClearScreen );

		{ Select from menu. }
		put_menu( row_mid, col_mid, main_menu, current_main );

		{ Set graphics mode, initialize turtle graphics, and
		  draw a border. }
		IF (current_main <> do_change_mode) THEN
			BEGIN
			return_code := _SetVideoMode( Mode );
			IF (_GrStatus <> _GrOk) THEN
				BEGIN
				GotoXY( col_mid - Length( mode_msg ) DIV 2, 1 );
				TextColor( Black );
				TextBackground( LightGray );
				Writeln( mode_msg );
				anykey := get_key( wait );
				END;
			bool_val := _DisplayCursor( False );
			_GetVideoConfig( vc );
			bool_val := initturtle;
			rectangle( 2 * tmaxx, 2 * tmaxy );
			END;

		{ Branch to menu choice. }
		CASE current_main OF
			do_quit :
				BEGIN
				bool_val := _DisplayCursor( True );
				return_code := _SetVideoMode( _DefaultMode );
				Halt( 0 );
				END;
			do_circles :
				circles;
			do_sphere :
				sphere;
			do_tunnel :
				BEGIN
				pendown( False );
				moveto( -tmaxx * 0.2, tmaxy * 0.15 );
				pendown( True );
				bool_val := polygons;
				WHILE (get_key( no_wait ) = 0) DO
					nextcolorvalue( default );  { Rotate palette. }
				END;
			do_spiral :
				BEGIN
				IF NOT spiral( get_random( 30, 80 ), get_random( 1, 5 ) )
				THEN WHILE (get_key( no_wait ) = 0) DO
					 nextcolorvalue( default );
				END;
			do_inverted_spiral:
				BEGIN
				return_code := nextcolorindex( 0 );
				IF (NOT inspiral( get_random( 8, 20 ),
					  get_random( 4, 22 ),
					  get_random( 3, 31 ) )) THEN
					WHILE (get_key( no_wait ) = 0) DO
						nextcolorvalue( default );
				END;
			do_bug :
				bug;
			do_adjust :
				adjust;
			do_change_mode :
				BEGIN
				IF (color) THEN _SetBkColor( Blue );
				_ClearScreen( _GClearScreen );
				put_menu( row_mid, col_mid, modes_menu, current_mode );
				Mode := modes_array[current_mode];
			END;  { Case change2 }
		END; { case }
	END; { WHILE true }
END.



