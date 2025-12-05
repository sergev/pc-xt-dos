PROGRAM objects;
{
  OBJECTS.PAS   Object demonstration program.

  This program graphically demonstrates the use of QuickPascal's
  object-oriented extensions. On the screen, you'll see bitmaps
  representing a jogger and one or more dogs.  Each time the
  jogger collides with a dog, he is bitten; after sufficient bites,
  he quits. You can press ESC to quit at any time.

  OBJECTS.PAS declares two types of objects: Sprite and Chaser
  (a subclass of Sprite). The jogger is a Sprite and the dogs
  are Chasers.  Associated with Sprite objects are a set of
  routines (methods) that initialize, move, draw, and erase; set
  speed and change direction; and detect collisions.  These
  methods apply to the jogger and to the dogs.  In addition,
  Chaser objects have a method called Chase, which causes the
  dogs to change direction when the jogger passes them.

}

{$M+}  { Enable method checking. }

USES
    MSGraph, Crt;

CONST
    escape     = Chr( 27 );
    quit_count = 10;
    max_dogs   = 10;
    max_steps  = 300;

    { ============== Color bitmaps =============================== }
    { Man running backward }
    b_man : ARRAY[1..376] OF Byte = (
                     21,0,31,0,0,0,0,1,224,0,1,224,0,1,224,0,0,0,0,3,
                     240,0,3,240,0,3,240,0,3,224,0,3,240,0,3,240,0,3,
                     240,0,7,224,0,6,240,0,6,240,0,7,240,0,7,224,0,7,
                     240,0,7,240,0,7,240,0,3,224,0,3,240,0,3,240,0,3,
                     240,0,3,224,0,3,224,0,3,224,0,3,224,0,1,224,0,1,
                     224,0,1,224,0,1,224,0,1,224,0,1,0,0,1,0,0,1,224,
                     0,1,224,0,0,224,0,0,224,0,1,224,0,7,240,0,4,112,
                     0,4,112,0,7,240,0,207,248,0,204,56,0,204,56,0,207,
                     248,0,223,252,0,216,28,0,216,28,0,223,252,0,255,
                     238,0,248,14,0,248,14,0,255,238,0,119,231,0,112,
                     7,0,112,7,0,119,231,0,39,238,0,32,14,0,32,14,0,
                     39,238,0,7,252,0,0,28,0,0,28,0,7,252,0,3,232,0,
                     0,8,0,0,8,0,3,232,0,0,0,0,0,0,0,3,224,0,3,224,0,
                     0,0,0,0,0,0,3,240,0,3,240,0,0,0,0,0,0,0,7,248,0,
                     7,248,0,0,0,0,0,0,0,15,252,0,15,252,0,30,30,0,30,
                     30,0,30,30,0,30,30,0,60,15,0,60,15,0,60,15,0,60,
                     15,0,56,7,128,56,7,128,56,7,128,56,7,128,28,3,128,
                     28,3,192,28,3,192,28,3,128,14,1,0,14,1,224,14,1,
                     224,14,1,0,6,0,0,7,3,192,7,3,192,6,0,0,0,0,0,31,
                     7,128,31,7,128,0,0,0,0,0,0,63,0,0,63,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0);

    { Man running forward }
    f_man : ARRAY[1..376] OF Byte = (
                     21,0,31,0,0,0,0,0,120,0,0,120,0,0,120,0,0,0,0,0,
                     252,0,0,252,0,0,252,0,0,124,0,0,252,0,0,252,0,0,
                     252,0,0,126,0,0,246,0,0,246,0,0,254,0,0,126,0,0,
                     254,0,0,254,0,0,254,0,0,124,0,0,252,0,0,252,0,0,
                     252,0,0,124,0,0,124,0,0,124,0,0,124,0,0,120,0,0,
                     120,0,0,120,0,0,120,0,0,120,0,0,120,0,0,120,0,0,
                     120,0,0,120,0,0,0,0,0,0,0,0,120,0,0,248,0,0,152,
                     0,0,152,0,0,248,0,1,252,16,1,156,16,1,156,16,1,
                     252,16,3,254,48,3,142,48,3,142,48,3,254,48,7,127,
                     112,7,7,112,7,7,112,7,127,112,14,127,224,14,3,224,
                     14,3,224,14,127,224,7,127,192,7,1,192,7,1,192,7,
                     127,192,3,254,128,3,128,128,3,128,128,3,254,128,
                     1,124,0,1,0,0,1,0,0,1,124,0,0,0,0,0,0,0,0,124,0,
                     0,124,0,0,0,0,0,0,0,0,252,0,0,252,0,0,0,0,0,0,0,
                     1,254,0,1,254,0,0,0,0,0,0,0,3,255,0,3,255,0,7,135,
                     128,7,135,128,7,135,128,7,135,128,15,3,192,15,3,
                     192,15,3,192,15,3,192,30,1,192,30,1,192,30,1,192,
                     30,1,192,28,3,128,60,3,128,60,3,128,28,3,128,8,
                     7,0,120,7,0,120,7,0,8,7,0,0,6,0,60,14,0,60,14,0,
                     0,6,0,0,0,0,30,15,128,30,15,128,0,0,0,0,0,0,0,15,
                     192,0,15,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

    { Dog running backward }
    b_dog : ARRAY[1..256] OF Byte = (
                     21,0,21,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,128,
                     0,1,128,0,0,0,0,0,0,0,0,192,0,0,192,0,0,0,0,0,0,
                     0,0,192,0,0,192,0,0,0,0,0,0,24,0,192,24,0,192,0,
                     0,0,0,0,0,24,0,192,24,0,192,0,0,0,0,0,0,28,0,192,
                     28,0,192,0,0,0,0,0,0,46,1,192,62,1,192,16,0,0,0,
                     0,0,255,255,192,255,255,192,0,0,0,0,0,0,255,255,
                     128,255,255,128,0,0,0,160,0,0,191,255,128,191,255,
                     128,0,0,0,64,0,0,95,255,128,95,255,128,0,0,0,0,
                     0,0,255,255,192,255,255,192,0,0,0,0,0,0,15,129,
                     224,15,129,224,0,0,0,0,0,0,13,193,240,13,193,240,
                     0,0,0,0,0,0,12,225,176,12,225,176,0,0,0,0,0,0,12,
                     113,160,12,113,160,0,0,0,0,0,0,12,225,160,12,225,
                     160,0,0,0,0,0,0,28,3,128,28,3,128,0,0,0,0,0,0,28,
                     3,128,28,3,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

    { Dog running forward }
    f_dog : ARRAY[1..256] OF Byte = (
                     21,0,21,0,0,0,0,8,0,0,8,0,0,0,0,0,0,0,0,24,0,0,
                     24,0,0,0,0,0,0,0,0,48,0,0,48,0,0,0,0,0,0,0,0,48,
                     0,0,48,0,0,0,0,0,0,0,0,48,1,128,48,1,128,0,0,0,
                     0,0,0,48,1,128,48,1,128,0,0,0,0,0,0,48,3,128,48,
                     3,128,0,0,0,0,0,0,56,7,64,56,7,192,0,0,128,0,0,
                     0,63,255,240,63,255,240,0,0,0,0,0,0,31,255,240,
                     31,255,240,0,0,0,0,0,80,31,255,208,31,255,208,0,
                     0,0,0,0,32,31,255,160,31,255,160,0,0,0,0,0,0,63,
                     255,240,63,255,240,0,0,0,0,0,0,120,31,0,120,31,
                     0,0,0,0,0,0,0,248,59,0,248,59,0,0,0,0,0,0,0,216,
                     115,0,216,115,0,0,0,0,0,0,0,88,227,0,88,227,0,0,
                     0,0,0,0,0,88,115,0,88,115,0,0,0,0,0,0,0,28,3,128,
                     28,3,128,0,0,0,0,0,0,28,3,128,28,3,128,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0);

    { ============== Monochrome bitmaps ============================= }

    { Man running backward }
    b_man_m : ARRAY[1..37] OF Byte = (
                     21,0,11,0,0,112,0,0,248,0,0,112,0,195,255,0,102,
                     112,192,28,112,48,0,248,192,3,142,0,6,3,128,14,
                     3,0,0,0,0);

    { Man running forward }
    f_man_m : ARRAY[1..37] OF Byte = (
                     21,0,11,0,0,224,0,1,240,0,0,224,0,15,252,48,48,
                     230,96,192,227,128,49,240,0,7,28,0,28,6,0,12,7,
                     0,0,0,0);

    { Dog running backward }
    b_dog_m : ARRAY[1..37] OF Byte = (
                     21,0,11,0,4,0,32,252,0,48,236,0,48,252,0,48,31,
                     255,240,255,255,240,31,255,224,25,134,96,25,134,
                     96,59,142,224,0,0,0);

    { Dog running forward }
    f_dog_m : ARRAY[1..37] OF Byte = (
                     21,0,11,0,64,2,0,192,3,240,192,3,112,192,3,240,
                     255,255,128,255,255,240,127,255,128,102,25,128,
                     102,25,128,119,29,192,0,0,0);

TYPE

    sprite = OBJECT
            { instance data }
            images : ARRAY[1..2] OF POINTER;
            isize  : Word;          { size of each bit image }
            x, y   : Word;          { position: ulhc }
            xe, ye : Word;          { extent }
            vx, vy : Integer;       { velocity }
            { methods }
            PROCEDURE initialize( size, xe, ye : Word );
            PROCEDURE set_images( b1, b2 : POINTER );
            PROCEDURE set_speed( vx, vy : Integer );
            PROCEDURE turn_around;
            PROCEDURE draw;
            PROCEDURE Move( x, y : Word );
            PROCEDURE erase_obj;
            PROCEDURE advance( xl, yt, xr, yb : Word );
            FUNCTION  hit_check( s : sprite ) : Boolean;
            PROCEDURE finalize;
            END; { sprite }

     chaser = OBJECT( sprite )
            PROCEDURE chase( x, y : Word );
            END;

VAR
    dogs        : ARRAY[1..max_dogs] OF chaser;
    man         : sprite;
    wl, wr,              { Maximum pixels left, right }
    wt, wb      : Word;  { Maximum pixels top, bottom }
    bcolor      : Word;  { Border color }
    delay_cnt   : Word;

{ ======================= sprite.initialize ======================
  This procedure sets initial values for sprite objects.
}

PROCEDURE sprite.initialize( size, xe, ye : Word );
BEGIN
    self.isize := size;
    self.xe := xe;
    self.ye := ye;
    self.vx := 0;
    self.vy := 0;
END;

{ ======================= sprite.set_images ======================
  This procedure initializes the image buffers for sprite objects
  with their bitmaps.
}
PROCEDURE sprite.set_images( b1, b2 : POINTER );
BEGIN
    self.images[1] := b1;
    self.images[2] := b2;
END;

{ ======================= sprite.set_speed ======================
  This procedure records the speed of a sprite object.
}
PROCEDURE sprite.set_speed( vx, vy : Integer );
BEGIN
    self.vx := vx;
    self.vy := vy;
END;

{ ======================= sprite.turn_around ======================
  This procedure is called when a sprite needs to reverse direction.
  The jogger changes direction whenever he reaches the edge of the
  screen.  The dogs change direction to follow the jogger.
}
PROCEDURE sprite.turn_around;
BEGIN
    self.erase_obj;
    self.vx := -self.vx;
    self.draw;
END;

{ ======================= sprite.move ==========================
  This procedure sets the position of a sprite object.
}

PROCEDURE sprite.Move( x, y : Word );
BEGIN
    self.x := x;
    self.y := y;
END;

{ ======================= sprite.draw ==========================
  This procedure draws a sprite object on the screen at the
  current position.
}

PROCEDURE sprite.draw;
VAR
    wi : Word;
BEGIN
    { Use sprite going forwards if x vector is positive;
      otherwise use sprite going backwards.
    }
    IF (self.vx > 0) THEN wi := 1 ELSE wi := 2;
    _PutImage( self.x, self.y, self.images[wi]^, _Gxor );
END;

{ ======================= sprite.erase_obj ==========================
  This procedure erases the most recently drawn sprite object.
  It is called before position is updated.  Since draw uses
  _Gxor, erase_obj simply redraws the same sprite in its current
  position.
}

PROCEDURE sprite.erase_obj;
BEGIN
    self.draw;
END;

{ ======================= sprite.advance ==========================
  This procedure erases the previously drawn sprite, sets a
  new position, and draws the next sprite.
}

PROCEDURE sprite.advance( xl, yt, xr, yb : Word );
BEGIN
    self.erase_obj;
    Inc( self.x, self.vx );
    Inc( self.y, self.vy );

    { Change direction if there's not enough room left on
      the screen for another sprite headed the same way.
    }
    IF ( self.x < xl) OR (self.x > (xr - self.xe)) THEN
        BEGIN
        self.vx := -self.vx;
        Inc( self.x, self.vx );
        END;
    IF (self.y < yt) OR (self.y > (yb - self.ye)) THEN
        BEGIN
        self.vy := -self.vy;
        Inc( self.y, self.vy );
        END;
    self.draw;
END;

{ ======================= sprite.hit_check ==========================
  This procedure checks to see whether two sprites have collided.
}

FUNCTION sprite.hit_check( s : sprite ) : Boolean;
VAR
    x1, y1, x2, y2 : Word;
    { ======================= pt_in_range ==========================
      This procedure returns True if any part of the sprites overlap.
    }

    FUNCTION pt_in_range( x, y : Word ) : Boolean;
    BEGIN
        pt_in_range := (x >= self.x) AND (x < (self.x + self.xe)) AND
                       (y >= self.y) AND (y < (self.y + self.ye));
    END;

BEGIN { sprite.hit_check }
    x1 := s.x;
    y1 := s.y;
    x2 := s.x + s.xe - 1;
    y2 := s.y + s.ye - 1;
    hit_check := pt_in_range( x1, y1 ) OR pt_in_range( x1, y2 ) OR
                 pt_in_range( x2, y1 ) OR pt_in_range( x2, y2 );
END;

{ ======================= sprite.finalize ==========================
  This procedure frees memory when the program terminates.
}

PROCEDURE sprite.finalize;
BEGIN
    FreeMem( self.images[1], self.isize );
    FreeMem( self.images[2], self.isize );
    self.isize := 0;
END;

{ ======================= chaser.chase ==========================
  This procedure is called for chaser objects (dogs).  It corrects
  their direction if the jogger has passed them.
}

PROCEDURE chaser.chase( x, y : Word );
VAR
    nvx, nvy : Integer;
BEGIN
    IF (x > self.x) THEN
        nvx := Abs( self.vx )
    ELSE
        nvx := -Abs( self.vx );
    IF (y > self.y) THEN
        nvy := Abs( self.vy )
    ELSE
        nvy := -Abs( self.vy );
    IF (nvx <> self.vx) THEN self.turn_around;
    self.vx := nvx;
    self.vy := nvy;
END;

{ ======================= get_a_speed ==========================
  This function returns a speed in the supplied range.
}

FUNCTION get_a_speed( min, max : Integer ) : Integer;
VAR
    range, num : Word;
BEGIN
    range := Abs( max - min );
    get_a_speed := max - Random( range );
END;

{ ======================= initialize ==========================
  This procedure initializes variables and graphics mode and
  draws the first objects.
}

PROCEDURE initialize;
VAR
    vidmode : Integer;
    vidrows : Integer;
    vc      : _VideoConfig;
    n       : Word;

BEGIN

    { Initialize random number generator. }
    Randomize;

    { Set up video information }
    DirectVideo := False;
    vidmode := _MaxResMode;
    vidrows := _SetVideoMode( vidmode );
    _GetVideoConfig( vc );
    IF (vc.Mode = _EResNoColor) THEN
        BEGIN
        vidrows := _SetVideoMode( _DefaultMode );
        Writeln( 'Program requires a high-resolution monochrome or '+
                 'color graphics video adapter' );
        Halt( 1 );
        END
    ELSE IF (vc.Mode = _EResColor) AND (vc.Memory = 64) THEN
        BEGIN    { Set correct mode on 64K EGA. }
        vidmode := _HRes16Color;
        vidrows := _SetVideoMode( _HRes16Color );
        _GetVideoConfig( vc );
        END;

    { Set maximum pixel numbers for left, right, top, bottom;
      set border color.
    }
    wl := 0;
    wr := vc.NumXPixels - 1;
    wt := vc.NumYPixels DIV vidrows + 1;
    wb := vc.NumYPixels - 1;
    bcolor := vc.NumColors - 1;

    { Set timing delay. }
    IF (vc.NumColors = 16) THEN delay_cnt := 0
    ELSE delay_cnt := 50;

    { Create images. }
    New( man );
    IF (vc.NumColors = 16) THEN  { use color bitmaps }
        BEGIN
        man.initialize( SizeOf( f_man ), 20, 30 );
        man.set_images( @f_man, @b_man );
        END
    ELSE      { use monochrome bitmaps }
        BEGIN
        man.initialize( SizeOf( f_man_m ), 20, 10 );
        man.set_images( @f_man_m, @b_man_m );
        END;
    man.set_speed( get_a_speed( -20, 20 ), get_a_speed( -10, 10 ) );
    man.move( Random( wr - 21 ) + 1, Random( ( wb - wt -30 ) - 1 ) + wt );

    { Create the dogs. }
    FOR n := 1 TO max_dogs DO
        BEGIN
        New( dogs[n] );
        IF (vc.NumColors = 16) THEN  { use color bitmaps }
            BEGIN
            dogs[n].initialize( SizeOf( f_dog ), 30, 20 );
            dogs[n].set_images( @f_dog, @b_dog );
            END
        ELSE     { use monochrome bitmaps }
            BEGIN
            dogs[n].initialize( SizeOf( f_dog_m ), 20, 10 );
            dogs[n].set_images( @f_dog_m, @b_dog_m );
            END;
        dogs[n].set_speed( 3, 1 );
   END;
END;  { procedure initialize }

{ ======================= finalize ==========================
  This procedure resets the video mode.
}

PROCEDURE finalize;
VAR
    vidrows : Integer;
BEGIN
    vidrows := _SetVideoMode( _DefaultMode );
END;

{ ======================= play ==========================
  This procedure does all the real work of the program.
}

PROCEDURE play;
VAR
    numdogs     : Word;
    n           : Word;
    bite_count  : Word;
    numstep     : Word;

BEGIN
    FOR numdogs := 1 TO max_dogs DO
        BEGIN
        { Clean up play area. }
        _ClearScreen( _GClearScreen );
        _SetColor( bcolor );
        _Rectangle( _GBorder, wl, wt, wr, wb );

        { Draw initial positions }
        man.draw;
        FOR n := 1 TO numdogs DO
            BEGIN
            { Select random initial positions for dogs. }
            dogs[n].Move( Random( wr - 31 ) + 1,
                          Random( (wb - wt - 20) - 1) + wt );
            dogs[n].draw;
            END;

        bite_count := 0;
        FOR numstep := max_steps DOWNTO 0 DO
            BEGIN
            { Does user want to quit? }
            IF KeyPressed THEN
                IF (ReadKey = escape) THEN
                    BEGIN
                    GotoXY( 1, 1 );
                    Write( 'Jogger quits!' );
                    Delay( 2000 );
                    Exit;
                    END; { both IF statements }

            { Advance dogs. Jogger is bitten every time he collides
              with a dog.
            }
            FOR n := 1 TO numdogs DO
                BEGIN
                IF man.hit_check( dogs[n] ) THEN
                    BEGIN
                    Inc( bite_count );
                    IF (bite_count > quit_count) THEN
                        BEGIN
                        GotoXY( 1, 1 );
                        Write( 'Jogger decides to take up another sport.' );
                        Delay( 2000 );
                        Exit;
                        END;
                    GotoXY( 1, 1 );
                    Writeln( 'Ouch! Dog bites jogger.' );
                    Delay( 500 );
                    GotoXY( 1, 1 );
                    Writeln( '                                         ' );
                    END; { if hit_check }
                dogs[n].advance( wl, wt, wr, wb );
                dogs[n].chase( man.x, man.y );
                END; { for n := 1 to numdog }

            { Advance jogger. }
            man.advance( wl, wt, wr, wb );
            Delay( delay_cnt );
            END; { for numstep ... }

        { Jogger's made it through a round. }
        GotoXY( 1, 1 );
        Write( 'Jogger makes it through ', numdogs * 2,' miles.' );
        Delay( 2000 );
        GotoXY( 1, 1 );
        Writeln( '                                      ' );
        END; { for numdogs ... }

    { Jogger has survived the battle of the dogs. }
    GotoXY( 1, 1 );
    Writeln( 'Jogger makes it home!' );
    Delay( 2000 );
    GotoXY( 1, 1 );
    Writeln( '                     ' );
END;

{ =========================== main program =========================== }

BEGIN
    initialize;
    play;
    finalize;
END.



