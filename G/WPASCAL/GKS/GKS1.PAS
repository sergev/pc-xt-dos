program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;

  begin
    gopks( 0 );
    gopwk( WK_ID, 0, MED_RES );
    gacwk( WK_ID );

    gtx( 0.5, 0.5, 'My first program' );

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
