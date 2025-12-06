program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;
    RADIUS      = 0.2;
    COLOUR      = 1;


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

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
