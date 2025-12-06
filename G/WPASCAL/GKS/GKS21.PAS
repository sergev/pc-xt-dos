program main( output );

  const
    WK_ID       = 14;
    EGA         = 10;
    HERCULES    = 11;

  var
    can_open : boolean;


  procedure gerhnd( errnum, funcid, errfil : integer );

    begin
      if( errnum = 26 )then     { cannot open workstation }
          can_open := FALSE
      else
        begin
          gerlog( errnum, funcid, errfil );
          pause;
        end;
    end;


  begin
    gopks( 0 );
    giega16( EGA );
    giherc( HERCULES );

    { First, try to open the Enhanced Graphics Adapter }
    can_open := TRUE;
    gopwk( WK_ID, 0, EGA );
    if( can_open = FALSE )then
      begin
        { There is no EGA, so try to open the
        { Hercules Monochrome Graphics Adapter }
        can_open := TRUE;
        gopwk( WK_ID, 0, HERCULES );
        if( can_open = FALSE )then
            pause;
      end;

    gacwk( WK_ID );

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
