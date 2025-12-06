program main( output );

  const
    WK_ID_A     = 14;
    WK_ID_B     = 15;
    MED_RES     = 2;
    META_OUT    = 5;

  begin
    gopks( 0 );
    gimo( META_OUT, 'new' );
    gopwk( WK_ID_A, 0, MED_RES );
    gopwk( WK_ID_B, 0, META_OUT );

    gacwk( WK_ID_A );
    gtx( 0.5, 0.7, 'Workstation A' );

    gacwk( WK_ID_B );
    gtx( 0.5, 0.5, 'Both' );

    gdawk( WK_ID_A );
    gtx( 0.5, 0.3, 'Workstation B' );

    gdawk( WK_ID_B );
    gclwk( WK_ID_A );
    gclwk( WK_ID_B );
    gclks;
  end.
