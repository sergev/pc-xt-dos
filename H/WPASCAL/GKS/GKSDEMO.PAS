program main( input, output );

  const
    NULL        = 0;
    CGA         = 4;
    EGA         = 6;
    PRT         = 30;
    MO          = 32;

  type
    h_alignment = ( H_NORMAL, H_LEFT, H_CENTRE, H_RIGHT );
    v_alignment = ( V_NORMAL, V_TOP, V_CAP, V_HALF, V_BASE, V_BOTTOM );
    string      = array [ 1..20 ] of char;
    real_array  = array [ 0..3 ] of real;
    str_array   = array [ 0..3 ] of string;

  var
    Active_ws   : integer;
    BACK, CYAN, MAG, WHITE : integer;

  procedure Define;

    begin
      gicga4( CGA );
      giega13( EGA );
      gigrpr( PRT );
      gimo( MO, 'demo.gks' );
      Active_ws := NULL;
    end;

  procedure SetDevice( dev : integer );

    begin
      if( dev = PRT )then
        begin
          BACK  := 0;
          CYAN  := 1;
          MAG   := 1;
          WHITE := 1;
        end
      else
        begin
          BACK  := 0;
          CYAN  := 1;
          MAG   := 2;
          WHITE := 3;
        end;
      Active_ws := 1;
      GOpWk( Active_ws, 0, dev );
      GAcWk( Active_ws );
      GSCR( Active_ws, BACK,  0, 0, 0 );
      GSCR( Active_ws, CYAN,  0, 1, 1 );
      GSCR( Active_ws, MAG,   1, 0, 1 );
      GSCR( Active_ws, WHITE, 1, 1, 1 );
    end;

  procedure CloseDevice;

    begin
      if( Active_ws <> NULL )then
        begin
          GDaWk( Active_ws );
          GClWk( Active_ws );
          Active_ws := NULL;
        end;
    end;

  procedure Wait;

    var
      start : real;

    begin
      start := time;
      while( time < start + 2 )do ; { wait for 2 seconds }
    end;

  procedure DoBorder;

    var
      x, y : real_array;

    begin
      GSWn( 1, 0, 1, 0, 1 );

      x[ 0 ] := 0;   y[ 0 ] := 0;
      x[ 1 ] := 0;   y[ 1 ] := 1;
      x[ 2 ] := 1;   y[ 2 ] := 1;
      x[ 3 ] := 1;   y[ 3 ] := 0;

      GSFAIS( 0 );
      GSFACI( WHITE );
      GFA( 4, x, y );
    end;

  procedure Title;

    begin
      GSTxAl( H_CENTRE, V_HALF );
      GSTxCI( CYAN );
      GSChH( 1/8 );
      GSWn( 1, -0.5, 0.5, 0, 1 );
      GSelNT( 1 );
      GTx( 0, 5/6, "WATCOM" );

      GSTxCI( WHITE );
      GSChH( 1/4 );
      GTx( 0, 0.5, "GKS" );

      GSChH( 1/8 );
      GSTxCI( MAG );
      GTx( 0, 1/6, "Graphics" );
    end;

  procedure OneBar( col_i, style_i : integer; xleft, xright, ybot, ytop : real );

    var
      ut_x, ut_y : real_array;

    begin
      ut_x[ 0 ] := xleft;          ut_y[ 0 ] := ybot;
      ut_x[ 1 ] := xright;         ut_y[ 1 ] := ybot;
      ut_x[ 2 ] := xright;         ut_y[ 2 ] := ytop;
      ut_x[ 3 ] := xleft;          ut_y[ 3 ] := ytop;

      GSFACI( col_i );
      GSFASI( style_i );
      GSFAIS( 3 );                        { hatch inside }
      GFA( 4, ut_x, ut_y );
      GSFAIS( 0 );
      GFA( 4, ut_x, ut_y );               { draw rectangles }
    end;

  procedure Bar( gt : string; n : integer; val1, val2 : real_array;
                 title : str_array; t1, t2 : string );

    var
      i, err : integer;
      maxval, scale_inc, wide, bar_size, yval, height, char_width : real;
      ut_x, ut_y, indep, lval1, lval2 : real_array;
      temp : string;

    begin
      maxval := 0;
      for i := 0 to n - 1 do
        begin
          if( val1[ i ] > maxval )then
              maxval := val1[ i ];
          if( val2[ i ] > maxval )then
              maxval := val2[ i ];
        end;
      CalcScale( maxval, scale_inc );
      for i := 0 to n - 1 do
        begin
          lval1[ i ] := val1[ i ] / maxval;
          lval2[ i ] := val2[ i ] / maxval;
        end;

    { Set the window }

      GSWn( 1, -0.2, 1.2, -0.2, 1.2 );   { room for axes & titles }
      GSelNT( 1 );

    { Draw the axes }

      ut_x[0] := 1;      ut_y[0] := 0;
      ut_x[1] := 0;      ut_y[1] := 0;
      ut_x[2] := 0;      ut_y[2] := 1;

      GSPlCI( WHITE );
      GPl( 3, ut_x, ut_y );

    { Draw the bars }

      wide := 1 / ( n + 1 );
      bar_size := wide / 3;

      for i := 0 to n - 1 do
        begin
          indep[ i ] := wide * ( i + 1 );
          OneBar( MAG, 1, indep[ i ] - bar_size, indep[ i ], 0, lval1[ i ] );
          OneBar( CYAN, 5, indep[ i ], indep[ i ] + bar_size, 0, lval2[ i ] );
        end;

    { Draw the legend }

      GSChH( bar_size * 0.9 );
      GSTxAl( H_LEFT, V_TOP );
      OneBar( MAG, 1, 0, bar_size, 1 - bar_size, 1 );
      GSTxCI( MAG );
      GTx( 2 * bar_size, 1, t1 );

      OneBar( CYAN, 5, 0, bar_size, 1 - 2 * bar_size, 1 - bar_size );
      GSTxCI( CYAN );
      GTx( 2 * bar_size, 1 - bar_size, t2 );

    { Put in the independent axis titles }

      GSTxCI( WHITE );
      GSTxAl( H_CENTRE, V_HALF );
      GSChH( 0.08 );
      GQChH( err, char_width );
      for i := 0 to n - 1 do
        begin
          GSChXp( wide / ( strlen( title[ i ] ) + 1 ) / char_width );
          GTx( indep[ i ], -0.05, title[ i ] );
        end;
      GSChXp( 1 );

    { Put in the dependent axis titles }

      GSTxAl( H_RIGHT, V_HALF );
      yval := 0;
      while( yval <= maxval )do
        begin
          rtos( yval, temp, 0 );
          ltrim( temp );
          GTx( -0.02, yval / maxval, temp );
          yval := yval + scale_inc;
        end;

    { Put in the graph title }

      GSTxCI( WHITE );
      GSChH( 0.2 );
      GQChH( err, char_width );
      GSChXp( 1 / strlen( gt ) / char_width );
      GSTxAl( H_LEFT, V_TOP );
      GTx( 0, 1.2, gt );
      GSChXp( 1 );

    end;

  procedure CalcScale( var maxv, scalev : real );

    var
      pow10, units : real;

    function ceil( value : real ) : integer;
      begin
        if( trunc( value ) = value )then
            ceil := value
        else
            ceil := trunc( value ) + 1;
      end;

    function power( a, b : real ) : real;
      begin
        power := exp( b * ln( a ) );
      end;

    function log10( value : real ) : real;
      begin
        log10 := ln( value ) / ln( 10 );
      end;

    begin
      pow10 := power( 10, ceil( log10( maxv ) ) - 1 );
      units := ceil( maxv / pow10 );
      maxv := units * pow10;
      if( units <= 3 )then
          scalev := pow10 / 2
      else if( units <= 5 )then
          scalev := pow10
      else
          scalev := 2 * pow10;
    end;

  procedure BarExample;

    var
      i : integer;
      expense, revenue : real_array;
      year : str_array;

    begin
      for i := 0 to 3 do
        begin
          revenue[ i ] := exp( i / 2 ) * 100;
          expense[ i ] := revenue[ i ] * 0.8;
          rtos( i + 1982, year[ i ], 0 );
          ltrim( year[ i ] );
        end;
      Bar( "Acme Inc.", 4, expense, revenue, year, "Expenses", "Revenues" )
    end;

  begin
    GOpKS( 0 );

    Define;

    SetDevice( CGA );
    DoBorder;
    Title;
    Wait;
    GClrWk( Active_ws, 0 );
    DoBorder;
    BarExample;
    Wait;

    GClrWk( Active_ws, 0 );
    GSVp( 1, 0, 0.5, 0.5, 1 );
    DoBorder;
    Title;
    GSVp( 1, 0.5, 1, 0, 0.5 );
    DoBorder;
    BarExample;
    GSVp( 1, 0.65, 0.85, 0.65, 0.85 );
    DoBorder;
    BarExample;

    Wait;
    CloseDevice;
    GClKS;
  end.
