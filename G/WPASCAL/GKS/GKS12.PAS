program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;
    META_WK_ID  = 21;
    META_IN_DEV = 47;

  var
    itmtyp, itmlen, itmsiz : integer;
    itmrec : packed array [ 1..100 ] of char;

  begin
    gopks( 0 );
    gopwk( WK_ID, 0, MED_RES );
    gacwk( WK_ID );
    gimi( META_IN_DEV, 'mymeta' );
    gopwk( META_WK_ID, 0, META_IN_DEV );

    ggtitm( META_WK_ID, itmtyp, itmlen );
    while( itmtyp <> 0 )do
      begin
        grditm( META_WK_ID, itmlen, itmsiz, itmrec );
        giitm( itmtyp, itmsiz, itmrec );
        ggtitm( META_WK_ID, itmtyp, itmlen );
      end;

    gclwk( META_WK_ID );
    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
