program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;
    COL         = 1;

  type
    real4 = array [ 1..4 ] of real;
    fill_area_interior_style = ( HOLLOW, SOLID, PATTERN, HATCH );

  const
    x = real4 [ 0.25, 0.25, 0.50, 0.50 ];
    y = real4 [ 0.25, 0.50, 0.50, 0.25 ];

  begin
    gopks( 0 );
    gopwk( WK_ID, 0, MED_RES );
    gacwk( WK_ID );

    gsfais( SOLID );
    gsfaci( COL );
    gscr( WK_ID, COL, 1, 0, 0 );
    gfa( 4, x, y );

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
