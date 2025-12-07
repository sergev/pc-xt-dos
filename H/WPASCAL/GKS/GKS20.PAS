program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;
    XC          = 0.2;
    YC          = 0.5;
    PIC_SIZE    = 1000;         { this constant may be made larger }

  type
    picture = packed array [ 1..PIC_SIZE ] of char;

  var
    x, y : real;
    ball : picture;


  procedure getpic( px, py, qx, qy : real; var pic : picture );

    const
      GETPIC    = -2;
      MAX_LEN   = 20;

    var
      error, across, down, len_data, len_pic : integer;
      ints : array [ 1..3 ] of integer;
      reals : array [ 1..2 ] of real;
      data : packed array [ 1..MAX_LEN ] of char;

    begin
      gqpxad( WK_ID, px, py, qx, qy, error, across, down );
      ints[ 1 ] := WK_ID;
      ints[ 2 ] := across;
      ints[ 3 ] := down;
      if( px < qx )then
          reals[ 1 ] := px
      else
          reals[ 1 ] := qx;
      if( py > qy )then
          reals[ 2 ] := py
      else
          reals[ 2 ] := qy;
      gprec( 3, ints, 2, reals, "", MAX_LEN, error, len_data, data );
      gesc( GETPIC, len_data, data, PIC_SIZE, len_pic, pic );
    end;


  procedure putpic_toggle( ulx, uly : real; var pic : picture );

    const
      NUM_PTS   = 1;
      SET_ACT   = -1;
      PUTPIC    = -5;

    var
      x, y : array [ 1..NUM_PTS ] of real;
      len : integer;
      ignore : packed array [ 1..1 ] of char;

    begin
      x[ 1 ] := ulx;
      y[ 1 ] := uly;
      gesc( SET_ACT, 3, 'xor', 0, len, ignore );
      ggdp( NUM_PTS, x, y, PUTPIC, PIC_SIZE, pic );
      gesc( SET_ACT, 3, 'replace', 0, len, ignore );
    end;


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

    gtx( 0.5, 0.5, "Watch the moving ball" );

    circle( XC, YC, 0.03, 1 );
    paint_boundary( XC, YC, 1, 1 );

    x := XC - 0.05;
    y := YC + 0.05;
    getpic( x, y, x + 0.1, y - 0.1, ball );

    putpic_toggle( x, y, ball );
    x := x + 0.05;
    while( x <= 1 )do
      begin
        putpic_toggle( x, y, ball );
        putpic_toggle( x, y, ball );
        x := x + 0.05;
      end;

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
