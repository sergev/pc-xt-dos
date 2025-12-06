program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;

  type
    real4 = array [ 1..4 ] of real;

  const
    x = real4 [ 0.25, 0.25, 0.50, 0.50 ];
    y = real4 [ 0.25, 0.50, 0.50, 0.25 ];

  begin
    gopks( 0 );
    gopwk( WK_ID, 0, MED_RES );
    gacwk( WK_ID );

    gpm( 2, x, y );

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
