program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;
    RADIUS      = 0.2;
    COLOUR      = 1;


  procedure paint_boundary( xs, ys : real; col_index, stop : integer );

    const
      PAINT     = -6;
      NUM_PTS   = 1;
      MAX_LEN   = 20;

    type
      fill_area_interior_style = ( HOLLOW, SOLID, PATTERN, HATCH );

    var
      x, y : array [ 1..NUM_PTS ] of real;
      error, len_data : integer;
      ints : array [ 1..1 ] of integer;
      reals : array [ 1..1 ] of real;
      data : packed array [ 1..MAX_LEN ] of char;

    begin
      x[ 1 ] := xs;
      y[ 1 ] := ys;
      gsfais( SOLID );
      gsfaci( col_index );
      ints[ 1 ] := stop;
      reals[ 1 ] := 0;
      gprec( 1, ints, 0, reals, "", MAX_LEN, error, len_data, data );
      ggdp( NUM_PTS, x, y, PAINT, len_data, data );
    end;


  procedure circle( xc, yc, radius : real; col_index : integer );

    const
      CIRCLE    = -1;
      NUM_PTS   = 2;

    var
      x, y : array [ 1..NUM_PTS ] of real;

    begin
      x[ 1 ] := xc;
      y[ 1 ] := yc;
      x[ 2 ] := xc + radius;
      y[ 2 ] := yc;
      gsplci( col_index );
      ggdp( NUM_PTS, x, y, CIRCLE, 0, "" );
    end;


  begin
    gopks( 0 );
    gopwk( WK_ID, 0, MED_RES );
    gacwk( WK_ID );

    circle( 0.5, 0.5, RADIUS, COLOUR );
    paint_boundary( 0.5, 0.5, COLOUR, COLOUR );

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
