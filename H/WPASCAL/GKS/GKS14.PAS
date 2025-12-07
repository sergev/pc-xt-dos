program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;

  type
    real4 = array [ 1..4 ] of real;

  const
    x = real4 [ 0.25, 0.25, 0.50, 0.50 ];
    y = real4 [ 0.25, 0.50, 0.50, 0.25 ];

  var
    error, colour_index : integer;
    message : packed array [ 1..80 ] of char;

  begin
    gopks( 0 );
    gopwk( WK_ID, 0, MED_RES );
    gacwk( WK_ID );

    gpm( 4, x, y );

    gqpx( WK_ID, 0.50, 0.50, error, colour_index );
    if( error <> 0 )then
      begin
        writeln( 'GKS Error ', error : 1 );
        gqermsg( error, 80, message );
        writeln( message );
      end
    else
      begin
        writeln( 'Centre pixel has colour index ', colour_index : 1 );
      end;

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
