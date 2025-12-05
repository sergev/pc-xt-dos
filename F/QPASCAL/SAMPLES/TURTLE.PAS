{ ============================= TURTLE ================================

  This unit contains procedures and functions that implement turtle
  graphics. Turtle graphics is a model for specifying relative movements
  of an imaginary pointer whose direction, color, visibility, and other
  attributes are given default values using turtle functions. To use the
  turtle module, specify Turtle in the USES section of your program.

  The following routines are public :

	InitTurtle      - Initiate turtle graphics
	Home            - Reset turtle defaults
	PenDown         - Set pen visibility
	PenState        - Get pen visibility
	FillState       - Get fill state
	FillOn          - Set fill state
	PenColor        - Set pen color index
	GetPenColor     - Get pen color index
	BorderColor     - Set border color index
	GetBorderColor  - Get border color index
	Turn            - Set direction relative to current
	TurnTo          - Set absolute direction
	GetTurn         - Get current turn angle
	Move            - Move in current direction
	MoveTo          - Move to absolute location
	Poly            - Draw a polygon
	Circle          - Draw a circle with center at current location
	Ellipse         - Draw an ellipse with center at current location
	Rectangle       - Draw a rectangle with center at current location
	ImageSize       - Get size of rectangle with top-left origin
	GetImage        - Get rectangular image with top-left origin
	PutImage        - Put rectangular image with top-left origin
	Paint           - Fill from the current location to border
	NextColorIndex  - Rotate to next color index
	NextColorValue  - Rotate to next color value
	OnScreen        - Report whether current location is on screen
	SetTWindow      - Set turtle window (viewport)
	GetTWindow      - Get turtle window (viewport)
	TUnit           - Get current turtle unit
	TCurX           - Get current x coordinate
	TCurY           - Get current y coordinate
	TurtleStat      - Get return status of turtle routine
}

UNIT turtle;

{ ===========================================================}
							INTERFACE
{ ===========================================================}

USES
	MSGraph;

TYPE
	degree = -360..360;
	twindow = RECORD
		Left, Right, Top, Bottom : Integer;
		END;

CONST
	{ Constants }
	circumference     = 360;
	halfcircumference = 180;
	maxitem = 20;
	default = -1;
	limited = 0;

	{ Screen Aspect - Adjust for your screen }
	TRatioYX : Real = 1.39;

VAR
	{ Global variables }
	tmaxx, tmaxy : Double;
	tcolorindexes : Integer;
	tcolorvalues  : Integer;

	{ ===================== Public Routines ====================== }

	PROCEDURE Move( dxy : Double );
	PROCEDURE moveto( x, y : Double );
	PROCEDURE turnto( cur_angle : degree );
	PROCEDURE turn( cur_angle : degree );
	PROCEDURE poly( n_sides : Integer; side_len : Double );
	FUNCTION  getturn : degree;

	PROCEDURE pendown( pen_on  : Boolean );
	PROCEDURE fillon( fill_on : Boolean );
	FUNCTION  penstate  : Boolean;
	FUNCTION  fillstate : Boolean;

	FUNCTION  initturtle : Boolean;
	PROCEDURE home;

	PROCEDURE bordercolor( border : Integer );
	FUNCTION  getbordercolor : Integer;
	PROCEDURE pencolor( cur_index : Integer );
	FUNCTION  getpencolor : Integer;

	FUNCTION  nextcolorindex( cur_index : Integer ) : Integer;
	PROCEDURE nextcolorvalue( action : Integer );

	PROCEDURE circle( r : Double );
	PROCEDURE ellipse( w, h : Double );
	PROCEDURE rectangle( w, h : Double );

	FUNCTION  imagesize( w, h : Double ) : LongInt;
	PROCEDURE getimage( w, h : Double; VAR buf );
	PROCEDURE putimage( VAR buf; Act : Integer );
	PROCEDURE paint;

	FUNCTION  onscreen( xcur, xMax, ycur, yMax : Double ) : Boolean;

	PROCEDURE settwindow( Left, Top, Right, Bottom : Integer );
	PROCEDURE gettwindow( VAR Left, Top, Right, Bottom : Integer );

	FUNCTION  tunit : Double;
	FUNCTION  tcurx : Double;
	FUNCTION  tcury : Double;

	FUNCTION  turtlestat : Boolean;

{ ===========================================================}
						 IMPLEMENTATION
{ ===========================================================}

{ Record for configuration and other data - hidden from user }
TYPE
	turtlerec = RECORD
		stat            : Boolean;
		pen_on          : Boolean;
		fill_on         : Integer;
		yunit           : Double;
		xcur, ycur      : Double;
		cur_index       : Integer;
		border_col      : Integer;
		cur_angle       : degree;
		palette_on      : Boolean;
		x_left, x_right : Integer;
		y_top, y_bottom : Integer
		END;

VAR
	long_colors : ARRAY[0..255] OF LongInt;
	vc : _VideoConfig;
	tc : turtlerec;


{ ======================== RGB ==============================
  Creates a composite red-green-blue color value from three
  byte values.
}
FUNCTION rgb( r, g, b : Word ) : LongInt;
BEGIN
	rgb := LongInt( LongInt( LongInt( b SHL 8 ) OR g ) SHL 8 ) OR r;
END; { rgb }

{ ======================== TurtleStat ============================
  Returns the completion status of various turtle routines.
}
FUNCTION turtlestat : Boolean;
BEGIN
	turtlestat := tc.stat;
END;  { turtlestat }

{ ======================== OnScreen ==============================
  Checks that a specified point is on the screen.
}
FUNCTION onscreen( xcur, xMax, ycur, yMax : Double ) : Boolean;
BEGIN
	IF  ((tc.xcur < -tmaxx) OR (tc.xcur > tmaxx)) OR
		((tc.ycur < -tmaxy) OR (tc.ycur > tmaxy)) THEN
		onscreen := False
	ELSE
		onscreen := True;
END; { onscreen }

{ ============================ initturtle ====================================
  Initializes all turtle defaults. This function should be called at
  least once (after _SetVideoMode and _GetVideoConfig) and additionally
  after any change to a new graphics mode.

  Params: vc - videoconfig record

  Return: True or False

  Uses:   tc record variable, long_colors array
}
FUNCTION initturtle : Boolean;
CONST
	Mode : Integer = -1;    { Impossible value }

VAR
	i, incr : Integer;
	r, g, b  : Word;

BEGIN
	_GetVideoConfig( vc  );

	{ Terminate if not graphics mode. }
	IF (vc.NumXPixels = 0) THEN
		BEGIN
		initturtle := False;
		Exit;
		END
	ELSE
		initturtle := True;

	{ If mode has changed, set window coordinates. }
	IF (Mode <> vc.Mode) THEN
		BEGIN
		Mode := vc.Mode;
		tc.x_left := 0;
		tc.y_top := 0;
		tc.x_right := vc.NumXPixels - 1;
		tc.y_bottom := vc.NumYPixels - 1;
		END;

	{ Set palette flag. }
	CASE vc.Adapter OF
		_MDPA,
		_CGA,
		_OCGA,
		_HGC :
			tc.palette_on := False;
		ELSE
			tc.palette_on := True
	END; { CASE }

	CASE vc.Mode OF
		_HResBW,
		_HercMono,
		_EResNoColor,
		_OResColor,
		_VRes2Color :
			BEGIN
			tcolorvalues := 0;
			tcolorindexes := 2;
			home;
			Exit;
			END;
									{ Active bits in this order:          }
		_MRes256Color :
			BEGIN                   { ???????? ??bbbbbb ??gggggg ??rrrrrr }
			incr := 12;
			tcolorvalues := 125;
			tcolorindexes := 125;
			END;

		_EResColor :
			IF (vc.Memory = 64) THEN
				BEGIN               { ???????? ??Bb???? ??Gg???? ??Rr???? }
				incr := 32;
				tcolorvalues := 16;
				tcolorindexes := 4;
				END
			ELSE
				BEGIN               { ???????? ??bb???? ??gg???? ??rr???? }
				incr := 16;
				tcolorvalues := 64;
				tcolorindexes := 16;
				END;

		_VRes16Color:
			BEGIN                   { ???????? ??bb???? ??gg???? ??rr???? }
			incr := 16;
			tcolorvalues := 64;
			tcolorindexes := 16;
			END;

		_MRes4Color,
		_MResNoColor:
			BEGIN                   { ???????? ??Bb???? ??Gg???? ??Rr???? }
			incr := 32;
			tcolorvalues := 16;
			tcolorindexes := 4;
			END;

		_MRes16Color,
		_HRes16Color :
			BEGIN                     { ???????? ??????Bb ??????Gg ??????Rr }
			incr := 32;
			tcolorindexes := 16;
			tcolorvalues := 16;
			END
		END; { CASE }

	{ Fill palette arrays. }
	i := 0;
	b := 0;
	WHILE (b < 64) DO
		BEGIN
		g := 0;
		WHILE (g < 64) DO
			BEGIN
			r := 0;
			WHILE (r < 64) DO
				BEGIN
				long_colors[i] := rgb( r, g, b );
				{ Special case: 6 bits for 16 colors (RGBI).
				  If both bits are on for any color, intensity is on.
				  If one bit is set for a color, that color is on.
				}
				IF (incr = 32) THEN
					long_colors[i+8] := long_colors[i] OR ( long_colors[i] SHR 1 );
				Inc( i );
				Inc( r, incr );
				END;
			Inc( g, incr );
			END;
		Inc( b, incr );
		END;

	long_colors[tcolorvalues - 1] := _BrightWhite;
	nextcolorvalue( default );

	home;
END; { initturtle }

{ ============================== home ==================================
  Resets turtle defaults. This procedure can be called if you have
  not changed the video mode, but you want to put the turtle back in
  the center of the window and restore all defaults. For example, you can
  change the absolute window corners and then call it to set a new
  turtle window.

  Params: vc - pointer to videoconfig structure

}
PROCEDURE home;
VAR
	prev : LongInt;

BEGIN

	_SetViewport( tc.x_left, tc.y_top, tc.x_right, tc.y_bottom );
	tmaxy := 500.0;
	tmaxx := tmaxy * TRatioYX;

	_SetWindow( False, -tmaxx, -tmaxy, tmaxx, tmaxy );
	IF (_GrStatus <> _GrOk) THEN
		BEGIN
		tc.stat := False;
		Exit;
		END;

	tc.xcur := 0.0;
	tc.ycur := 0.0;
	_MoveTo_w( tc.xcur, tc.ycur );
	turnto( 0 );
	pendown( True );
	fillon( False );

	prev := _RemapPalette( tcolorindexes - 1, _BrightWhite );
	bordercolor( tcolorindexes - 1 );
	pencolor( tcolorindexes - 1 );

END; { home }

{ ============================== pendown ==================================

  Sets the visibility of the pen used by Move and MoveTo.

  Params: pen_on - True or False
}
PROCEDURE pendown( pen_on : Boolean );
BEGIN
	tc.pen_on := pen_on;
END;  { pendown }

{ ============================== penstate ==================================
  Gets the visibility of the pen used by Move and MoveTo.

  Return: True or False
}
FUNCTION penstate : Boolean;
BEGIN
	penstate := tc.pen_on;
END;  { penstate }

{ ============================== fillon ==================================
  FillOn - Sets the state of Filling figures such as Rectangle,
  Circle, and Ellipse.

  Params: fill_on - True or False
}
PROCEDURE fillon( fill_on : Boolean );
BEGIN
	IF fill_on THEN
		tc.fill_on := _GFillInterior
	ELSE
		tc.fill_on := _GBorder;
END;  { fillon }

{ ============================== fillstate ==================================
  Gets the state of Painting figures such as Rectangle,
  Circle, and Ellipse.

  Return: True or False
}
FUNCTION fillstate : Boolean;
BEGIN
	IF (tc.fill_on = _GBorder) THEN
		fillstate := False
	ELSE
		fillstate := True;
END;  { fillstate }

{ ============================== pencolor ==================================
 Sets the color index of the pen.

  Params: cur_index - a color index
}
PROCEDURE pencolor( cur_index : Integer );
BEGIN
	tc.cur_index := cur_index;
	_SetColor( tc.cur_index );
END;  { pencolor }

{ ============================== getpencolor  ===========================
  Gets the color index of the pen.

  Return: current color index
}
FUNCTION getpencolor : Integer;
BEGIN
	getpencolor := tc.cur_index;
END;  { getpencolor }

{ ============================== bordercolor  ===========================
  BorderColor - Sets the color index of the border that will be recognized
  by fills.

  Params: border_col - any color index
}
PROCEDURE bordercolor( border : Integer );
BEGIN
	tc.border_col := border;
END;   { bordercolor }

{ ========================== getbordercolor ============================
  Gets the color index of the border that will be recognized by fills.

  Return: current border color index
}
FUNCTION getbordercolor : Integer;
BEGIN
	getbordercolor := tc.border_col;
END;  { getbordercolor }

{ ============================== turn ==================================
  Sets a new direction relative to the current direction.

  Params: cur_angle - a positive (clockwise) or negative (counterclockwise)
			angle in degrees
}
PROCEDURE turn( cur_angle : degree );
BEGIN
	tc.cur_angle := (tc.cur_angle + cur_angle) MOD circumference;
END;  { turn }


{ ============================== turnto ==================================
  Sets a new absolute direction.

  Params: cur_angle - a positive (clockwise) or negative (counterclockwise)
			angle in degrees (0 points to 12 o'clock)

  Uses:   tc
}
PROCEDURE turnto( cur_angle : degree );
BEGIN
	IF (cur_angle < 0) THEN
		tc.cur_angle := circumference - (cur_angle MOD circumference)
	ELSE
		tc.cur_angle := cur_angle MOD circumference;
END;  { turnto }


{ ============================== getturn ==================================
  Gets the current absolute angle.

  Return: Angle between 0 and 359
}
FUNCTION getturn : degree;
BEGIN
	getturn := tc.cur_angle;
END;  { getturn }


{ ============================== move ==================================
  Moves from the current position in the current direction for a
  specified distance. A line is drawn if the pen is down. The current
  position is reset to the destination.

  Params: dxy - difference between current xy and new xy
}
PROCEDURE move( dxy : Double );
VAR
	dx, dy, angt : Double;

BEGIN
	angt := (tc.cur_angle - 90) * (Pi / halfcircumference);
	dx := dxy * Cos( angt );
	dy := dxy * Sin( angt );

	IF tc.pen_on THEN
		_LineTo_w( tc.xcur + dx, tc.ycur + dy )
	ELSE
		_MoveTo_w( tc.xcur + dx, tc.ycur + dy );

	tc.xcur := tc.xcur + dx;
	tc.ycur := tc.ycur + dy;

	IF onscreen( tc.xcur, tmaxx, tc.ycur, tmaxy ) THEN
		tc.stat := True
	ELSE
		tc.stat := False;
END;  { move }

{ ============================== moveto ==================================
  Moves from the current position to a specified position. A
  line is drawn if the pen is down. The current position is reset to the
  destination. The current direction is not changed.

  Params: x and y - destination position
}
PROCEDURE moveto( x, y : Double );
BEGIN
	IF tc.pen_on THEN
		_LineTo_w( x, y )
	ELSE
		_MoveTo_w( x, y );
	tc.xcur := x;
	tc.ycur := y;
	IF onscreen( tc.xcur, tmaxx, tc.ycur, tmaxy ) THEN
		tc.stat := True
	ELSE
		tc.stat := False;
END;  { moveto }

{ ============================== poly ==================================
  Draws a polygon.

  Params: n_sides - count of polygon sides
		  side_len - distance of each side

  Return: 0 if any part of polygon is off screen, nonzero if on screen
}
PROCEDURE poly( n_sides : Integer; side_len : Double );
VAR
	i, angle   : Integer;
	on, pen_on : Boolean;

BEGIN
	on := True;
	pendown( True );
	pen_on := turtlestat;
	angle := Round( 360 / n_sides );

	FOR i := 1 TO n_sides DO
		BEGIN
		Move( side_len );
		on := on AND turtlestat;
		turn( angle );
		END;

	pendown( pen_on );
	tc.stat := on;
END;  { poly }

{ ============================== nextcolorindex ========================
  Rotate to next color index. First attribute (normally background) and
  last attribute (white) are skipped.

  Params: cur_index - Specify DEFAULT to use color index from last call,
			or specify a new color to rotate from

  Return: rotated color index

  Uses:   tc
}
FUNCTION nextcolorindex( cur_index : Integer ) : Integer;
CONST
	prev_index : Integer = 0;

BEGIN
	{ Assign new current value if supplied. }
	IF (cur_index <> default) THEN
		prev_index := cur_index;

	{ Toggle for 2-color modes; rotate for multi-color modes. }

	IF (tcolorindexes = 2) THEN
		BEGIN
		prev_index := NOT prev_index;
		nextcolorindex := prev_index;
		END
	ELSE
		BEGIN
		Inc( prev_index );
		prev_index := prev_index MOD (tcolorindexes - 1);
		nextcolorindex := prev_index;
		END;
END; { nextcolorindex }

{ =========================== nextcolorvalue ===========================
  Rotate to next color value for adapters (EGA and higher) that support
  remappable palettes.

  Params: action - DEFAULT (rotate all) or LIMITED (rotate first
				   14 only)
}
PROCEDURE nextcolorvalue( action : Integer );
CONST
	cur_val   : LongInt = 1;
	cur_index : Integer = 1;
VAR
	temp_val, i : Integer;
	prev    : LongInt;

BEGIN

	{ Ignore modes with no palette values. }

	IF ((NOT tc.palette_on) OR (tcolorvalues = 0)) THEN
		Exit;

	{ Increment and rotate color value index. }
	Inc( cur_val );
	temp_val := cur_val MOD (tcolorvalues - 2) + 1;

	{ DEFAULT - Remap all color indexes, 14 at a time. For most modes,
	  this is all the indexes except first and last. For 256-color
	  mode, rotating all available indexes would be too slow.
	}
	IF (action = default) THEN
		FOR i := 1 TO 14 DO
			BEGIN
			prev := _RemapPalette( (cur_index MOD (tcolorindexes - 2 ) ) + 1,
								   long_colors[(temp_val MOD (tcolorvalues - 2)) + 1] );
			Inc( cur_index );
			Inc( temp_val );
			END
	{ LIMITED - Rotate only the first 14 color indexes. }
	ELSE
		FOR i := 1 TO 14 DO
			BEGIN
			prev := _RemapPalette( i,
						long_colors[(temp_val MOD (tcolorvalues - 1)) + 1] );
			Inc( temp_val );
			END;

END; { nextcolorvalue }

{ =============================== circle ===========================
  Put a circle with radius <r> at current location.
}
PROCEDURE circle( r : Double );
BEGIN
	_Ellipse_w( tc.fill_on, tc.xcur - r, tc.ycur - r,
						   tc.xcur + r, tc.ycur + r );
	tc.stat := (_GrStatus = _GrOk);
END;  { circle }

{ ============================== ellipse==================================
  Puts  an ellipse with width <w> and height <h> at current location.
}
PROCEDURE ellipse( w, h : Double );
BEGIN
	_Ellipse_w( tc.fill_on, tc.xcur - (w / 2), tc.ycur - (h / 2),
						   tc.xcur + (w / 2), tc.ycur + (h / 2) );
	tc.stat := (_GrStatus = _GrOk);
END;  { ellipse }

{ ============================== rectangle ==================================
  Puts the center of a rectangle with width <w> and height <h>
  at current location.
}
PROCEDURE rectangle( w, h : Double );
BEGIN
	_Rectangle_w( tc.fill_on, tc.xcur - (w / 2), tc.ycur - (h / 2),
							 tc.xcur + (w / 2), tc.ycur + (h / 2) );
	tc.stat := (_GrStatus = _GrOk);
END;  { rectangle }

{ ============================== imagesize ============================
  Gets the size of an image with width <w> and height <h>
  with left-top at current location.
}
FUNCTION imagesize( w, h : Double ) : LongInt;
BEGIN
	imagesize := _ImageSize_w( tc.xcur,     tc.ycur,
							   tc.xcur + w, tc.ycur + h );
	tc.stat := (_GrStatus = _GrOk);
END;   { imagesize }

{ ============================== getimage ==============================
  Gets an image with width <w> and height <h> with left-top
  at current location. Returns image buffer.
}
PROCEDURE getimage( w, h : Double; VAR buf );
BEGIN
	_GetImage_w( tc.xcur, tc.ycur, tc.xcur + w, tc.ycur + h, buf );
	tc.stat := (_GrStatus = _GrOk);
END;  { getimage }

{ ============================== putimage ==================================
  Puts the top-left corner of a specified image at current location
  using a specified action (_GPSET, _GPRESET, _GAND, _GOR, _GXOR).
}
PROCEDURE putimage( VAR buf; act : Integer );
BEGIN
	_PutImage_w( tc.xcur, tc.ycur, buf, act );
	tc.stat := (_GrStatus = _GrOk);
END;   { putimage }

{ ============================== paint ================================
  Paints from the current location to the border.
}
PROCEDURE paint;
BEGIN
	_FloodFill_w( tc.xcur, tc.ycur, tc.border_col );
	tc.stat := (_GrStatus = _GrOk);
END;  { paint }

{ ============================== gettwindow ==============================
  Returns the coordinates of the turtle window (viewport).
}
PROCEDURE gettwindow( VAR Left, Top, Right, Bottom : Integer );
BEGIN
	Left    := tc.x_left;
	Right   := tc.x_right;
	Top     := tc.y_top;
	Bottom  := tc.y_bottom;
END;    { gettwindow }

{ ============================== settwindow ==============================
  Sets the coordinates of the turtle window (viewport) .
}
PROCEDURE settwindow( Left, Top, Right, Bottom : Integer );
BEGIN
	tc.x_left   := Left;
	tc.x_right  := Right;
	tc.y_top    := Top;
	tc.y_bottom    := Bottom;
	_SetViewport( tc.x_left, tc.y_top, tc.x_right, tc.y_bottom );
	tc.stat := (_GrStatus = _GrOk);

END;  { settwindow }

{ ============================== tunit ==============================
  Returns the value of a turtle unit.
}
FUNCTION tunit : Double;
VAR
	xy1, xy2  : _WXYCoord;

BEGIN
	_GetWindowCoord( 1, 1, xy1 );
	_GetWindowCoord( 1, 2, xy2 );
	tunit := xy2.Wy - xy1.Wy;
END;   { tunit }

{ ============================== tcurx ==============================
  Returns the current x coordinate.
}
FUNCTION tcurx : Double;
BEGIN
	tcurx := tc.xcur;
END;  { tcurx }

{ ============================== tcury ==============================
  Returns the current y-coordinate.
}
FUNCTION tcury : Double;
BEGIN
	tcury := tc.ycur;
END;  { tcury }

END. { UNIT }



