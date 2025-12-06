{
    This file contains a collection of Pascal procedures and functions
    which can be used to generate graphics pictures. The procedures and
    functions provide an interface to GKS.

    The program including this file must call the procedure INIT before
    drawing any graphic images and should call FINISH when all drawing is
    completed.
}

const
    WK_ID               = 1;
    WK_LST              = 6;
    MT_DOT              = 1;    { marker types }
    MT_PLUS             = 2;
    MT_ASTERISK         = 3;
    MT_CIRCLE           = 4;
    MT_CROSS            = 5;
    MT_DIAMOND          = -1;
    TF_LOW_QUALITY      = 1;    { text fonts }
    TF_HIGH_QUALITY     = 2;
    BLACK               = 1;    { colours }
    RED                 = 2;
    GREEN               = 3;
    YELLOW              = 4;
    BLUE                = 5;
    MAGENTA             = 6;
    CYAN                = 7;
    WHITE               = 8;
    GRAY                = 9;
    LIGHT_RED           = 10;
    LIGHT_GREEN         = 11;
    LIGHT_YELLOW        = 12;
    LIGHT_BLUE          = 13;
    LIGHT_MAGENTA       = 14;
    LIGHT_CYAN          = 15;
    BRIGHT_WHITE        = 16;

type
    fill_style          = ( HOLLOW, SOLID, PATTERN, HATCH );
    horiz_align         = ( HA_NORMAL, HA_LEFT, HA_CENTRE, HA_RIGHT );
    vert_align          = ( VA_NORMAL, VA_TOP, VA_CAP, VA_HALF,
                                               VA_BASE, VA_BOTTOM );
    text_path           = ( TP_RIGHT, TP_LEFT, TP_UP, TP_DOWN );
    text_prec           = ( TP_STRING, TP_CHAR, TP_STROKE );

const
    PI                  = 3.141592654;
    GKSactions          = 'rxao';
    PIC_SIZE            = 10000;

type
    text_str    = packed array [ 1..80 ] of char;
    picture     = packed array [ 1..PIC_SIZE ] of char;
    colour_tab  = array [ 1..16 ] of real;
    dev_tab     = array [ 1..WK_LST ] of integer;

const
    red_level   = colour_tab[ 0.0000, 0.6667, 0.0000, 0.6667,
                              0.0000, 0.6667, 0.0000, 0.6667,
                              0.3333, 1.0000, 0.3333, 1.0000,
                              0.3333, 1.0000, 0.3333, 1.0000 ];
    green_level = colour_tab[ 0.0000, 0.0000, 0.6667, 0.3333,
                              0.0000, 0.0000, 0.6667, 0.6667,
                              0.3333, 0.3333, 1.0000, 1.0000,
                              0.3333, 0.3333, 1.0000, 1.0000 ];
    blue_level  = colour_tab[ 0.0000, 0.0000, 0.0000, 0.0000,
                              0.6667, 0.6667, 0.6667, 0.6667,
                              0.3333, 0.3333, 0.3333, 0.3333,
                              1.0000, 1.0000, 1.0000, 1.0000 ];
    GKSsel      = dev_tab[ 13, 9, 14, 5, 1, 10 ];

var
    GKSerr, GKStemp, GKSpw, GKSph : integer;
    GKSwidth, GKSheight : real;
    GKSx, GKSy : array [ 0..3 ] of real;
    GKSint : array [ 0..2 ] of integer;
    GKStemp_str : packed array [ 1..20 ] of char;

procedure init( device : integer );

    var
        i : integer;

    begin
        geclks;
        gopks( 0 );
        gicga4( 1 );
        gicga6( 2 );
        gijr8( 3 );
        gijr9( 4 );
        gijr10( 5 );
        giega13( 6 );
        giega14( 7 );
        giega15( 8 );
        giega16( 9 );
        giherc( 10 );
        gigrpr( 11 );
        givga17( 12 );
        givga18( 13 );
        givga19( 14 );
        { if the user specified device '0', try to find an available }
        { workstation, otherwise open the specified workstation }
        if( device = 0 )then
          begin
            i := 1;
            while( i <= WK_LST )do
              begin
                GKSerr := 0;
                device := GKSsel[ i ];
                gopwk( WK_ID, 0, device );
                if( GKSerr = 0 )then
                    i := WK_LST; { exit the loop }
                i := i + 1;
              end;
          end
        else
          begin
            GKSerr := 0;
            gopwk( WK_ID, 0, device );
          end;
        if( GKSerr <> 0 )then
          begin
            writeln( 'Graphics adapter not present' );
            pause;
          end;
        gacwk( WK_ID );
        gqdsp( device, GKSerr, GKStemp, GKSwidth, GKSheight, GKSpw, GKSph );
        gsmk( MT_DOT );
        gstxal( HA_LEFT, VA_BOTTOM );
    end;

procedure finish;

    begin
        gdawk( WK_ID );
        gclwk( WK_ID );
        gclks;
    end;

procedure gerhnd( errnum, gksrtn, errfil : integer );

    begin
        GKSerr := errnum;
        if( errnum <> 26 )then
          begin
            { display the error message and stop execution }
            gerlog( errnum, gksrtn, errfil );
            pause;
          end;
    end;

procedure ell_arc( x, y, x_axis, y_axis : real; colour, start, fini : integer );

    begin
        GKSx[ 0 ] := x;
        GKSy[ 0 ] := y;
        { calculate a point on the axis away from the start point }
        if( abs( sin( start * PI / 180 ) ) < 0.5 )then
          begin
            GKSx[ 1 ] := x;
            GKSy[ 1 ] := y + y_axis;
          end
        else
          begin
            GKSx[ 1 ] := x + x_axis;
            GKSy[ 1 ] := y;
          end;
        { calculate the start and end points on the ellipse }
        GKSx[ 2 ] := x + cos( start * PI / 180 ) * x_axis;
        GKSy[ 2 ] := y + sin( start * PI / 180 ) * y_axis;
        GKSx[ 3 ] := x + cos( fini * PI / 180 ) * x_axis;
        GKSy[ 3 ] := y + sin( fini * PI / 180 ) * y_axis;
        gsplci( colour );
        ggdp( 4, GKSx, GKSy, -4, 0, "" );
    end;

procedure circ_arc( x, y, radius : real; colour, start, fini : integer );

    begin
        ell_arc( x, y, radius, radius * GKSwidth / GKSheight,
                 colour, start, fini );
    end;

procedure block( x1, y1, x2, y2 : real; colour : integer );

    begin
        GKSx[ 0 ] := x1; GKSy[ 0 ] := y1;
        GKSx[ 1 ] := x1; GKSy[ 1 ] := y2;
        GKSx[ 2 ] := x2; GKSy[ 2 ] := y2;
        GKSx[ 3 ] := x2; GKSy[ 3 ] := y1;
        gsfais( SOLID );
        gsfaci( colour );
        gfa( 4, GKSx, GKSy );
    end;

procedure box( x1, y1, x2, y2 : real; colour : integer );

    begin
        GKSx[ 0 ] := x1; GKSy[ 0 ] := y1;
        GKSx[ 1 ] := x1; GKSy[ 1 ] := y2;
        GKSx[ 2 ] := x2; GKSy[ 2 ] := y2;
        GKSx[ 3 ] := x2; GKSy[ 3 ] := y1;
        gsfais( HOLLOW );
        gsfaci( colour );
        gfa( 4, GKSx, GKSy );
    end;

procedure circle( x, y, radius : real; colour : integer );

    begin
        GKSx[ 0 ] := x; GKSy[ 0 ] := y;
        GKSx[ 1 ] := x + radius; GKSy[ 1 ] := y;
        gsplci( colour );
        ggdp( 2, GKSx, GKSy, -1, 0, "" );
    end;

procedure clearscreen;

    begin
        gclrwk( WK_ID, 1 );
    end;

procedure colourmap( colour_index, colour : integer );

    var
      err, numcol, colidx : integer;

    begin
        { set red, green and blue intensities for the given colour }
        gqeci( WK_ID, -1, err, numcol, colidx );
        if( colour_index < numcol )then
            gscr( WK_ID, colour_index, red_level[ colour ],
                         green_level[ colour ], blue_level[ colour ] );
    end;

procedure ellipse( x, y, x_axis, y_axis : real; colour : integer );

    begin
        GKSx[ 0 ] := x;           GKSy[ 0 ] := y;
        GKSx[ 1 ] := x + x_axis;  GKSy[ 1 ] := y;
        GKSx[ 2 ] := x;           GKSy[ 2 ] := y + y_axis;
        gsplci( colour );
        ggdp( 3, GKSx, GKSy, -3, 0, "" );
    end;

procedure getpic( x1, y1, x2, y2 : real; var pic : picture );

    begin
        GKSint[ 0 ] := 1;
        { calculate the number of pixels in the box }
        GKSint[ 1 ] := round( ( x2 - x1 ) * GKSpw ) + 1;
        GKSint[ 2 ] := round( ( y1 - y2 ) * GKSph ) + 1;
        GKSx[ 0 ] := x1;
        GKSx[ 1 ] := y1;
        gprec( 3, GKSint, 2, GKSx, "", 20, GKSerr, GKStemp, GKStemp_str );
        gesc( -2, GKStemp, GKStemp_str, PIC_SIZE, GKStemp, pic );
    end;

procedure line( x1, y1, x2, y2 : real; colour : integer );

    begin
        GKSx[ 0 ] := x1; GKSy[ 0 ] := y1;
        GKSx[ 1 ] := x2; GKSy[ 1 ] := y2;
        gsplci( colour );
        gpl( 2, GKSx, GKSy );
    end;

procedure paint( x, y : real; colour : integer );

    begin
        GKSx[ 0 ] := x;
        GKSy[ 0 ] := y;
        gsfaci( colour );
        gsfais( SOLID );
        ggdp( 1, GKSx, GKSy, -6, 0, "" );
    end;

procedure paint_boundary( x, y : real; colour, border : integer );

    begin
        GKSx[ 0 ] := x;
        GKSy[ 0 ] := y;
        { create a data record containing the border colour }
        GKSint[ 0 ] := border;
        gprec( 1, GKSint, 0, GKSx, "", 20, GKSerr, GKStemp, GKStemp_str );
        gsfaci( colour );
        gsfais( SOLID );
        ggdp( 1, GKSx, GKSy, -6, GKStemp, GKStemp_str );
    end;

procedure putdot( x, y : real; colour : integer );

    begin
        GKSx[ 0 ] := x;
        GKSy[ 0 ] := y;
        gspmci( colour );
        gpm( 1, GKSx, GKSy );
    end;

procedure putpic( x, y : real; var pic : picture );

    begin
        GKSx[ 0 ] := x;
        GKSy[ 0 ] := y;
        ggdp( 1, GKSx, GKSy, -5, PIC_SIZE, pic );
    end;

procedure putpic_action( x, y : real; var pic : picture; action : integer );

    begin
        { set the plotting representation according to the given action }
        substr( GKSactions, 1, action + 1, GKStemp_str );
        gesc( -1, 1, GKStemp_str, 1, GKStemp, GKStemp_str );
        putpic( x, y, pic );
        { reset the plotting representation }
        gesc( -1, 1, "r", 1, GKStemp, GKStemp_str );
    end;

procedure slice( x, y, radius : real; colour, start, fini : integer );

    var
        angle : real;

    begin
        circ_arc( x, y, radius, colour, start, fini );
        { join the midpoint to the ends of the arc }
        GKSx[ 1 ] := GKSx[ 2 ];
        GKSy[ 1 ] := GKSy[ 2 ];
        gpl( 2, GKSx, GKSy );
        GKSx[ 1 ] := GKSx[ 3 ];
        GKSy[ 1 ] := GKSy[ 3 ];
        gpl( 2, GKSx, GKSy );
        { calculate a point in the slice, half the angle, 80% of the length }
        angle := ( ( start + fini ) / 2 ) * ( PI / 180 );
        paint( x + 0.8 * radius * cos( angle ),
               y + 0.8 * radius * sin( angle ) * GKSwidth / GKSheight, colour );
    end;

procedure text( x, y : real; str : text_str; colour : integer );

    begin
        gstxci( colour );
        gtx( x, y, str );
    end;

function getdot( x, y : real ) : integer;

    begin
        gqpx( WK_ID, x, y, GKSerr, GKStemp );
        getdot := GKStemp;
    end;
