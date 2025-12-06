program main( output );

  const
    WK_ID       = 14;
    META_OUT    = 23;
    COMPACT     = 1;

  type
    real4 = array [ 1..4 ] of real;

  const
    x = real4 [ 0.25, 0.25, 0.50, 0.50 ];
    y = real4 [ 0.25, 0.50, 0.50, 0.25 ];

  begin
    gopks( 0 );
    gimo( META_OUT, 'mymeta' );
    gopwk( WK_ID, COMPACT, META_OUT );
    gacwk( WK_ID );

    gpm( 4, x, y );

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
