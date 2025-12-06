program main( input, output );

type
    colours = ( black, blue, green, cyan, red, magenta, brown, white,
                gray, light_blue, light_green, light_cyan, light_red,
                light_magenta, yellow, bright_white );
    array12 = array [ 1..12 ] of integer;
    names = array [ 1..12 ] of packed array [ 1..10 ] of char;

const
    february = 2;
    days_in_month = array12 [ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 ];
    jump = array12 [ 1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6 ];
    month_name = names [ [ 'January' ], [ 'February' ], [ 'March' ],
                         [ 'April' ],   [ 'May' ],      [ 'June' ],
                         [ 'July' ],    [ 'August' ],   [ 'September' ],
                         [ 'October' ], [ 'November' ], [ 'December' ] ];

var
    today : packed array [ 1..8 ] of char;
    year, month, day : integer;

procedure box( row, col, width, height : integer );

  var
    i : integer;

  begin
    poscursor( row, col );
    write( 'Ú' );
    for i := 1 to width do
        write( 'Ä' );
    write( '¿' );

    poscursor( row + 1, col );
    write( '³', ' ' : width, '³' );

    poscursor( row + 2, col );
    write( 'Ã' );
    for i := 1 to width do
        write( 'Ä' );
    write( '´' );

    for i := 3 to height do
      begin
        poscursor( row + i, col );
        write( '³', ' ' : width, '³' );
      end;

    poscursor( row + height + 1, col );
    write( 'À' );
    for i := 1 to width do
        write( 'Ä' );
    write( 'Ù' );
  end;

procedure calendar( month, year, row, col, wide, today : integer );

  var
    start, days, i : integer;

  begin
    start := year + year div 4 + jump[ month ];
    if( ( year mod 4 = 0 ) and ( month <= february ) )then
        start := start - 1;
    start := start mod 7 + 1;
    if( wide = 4 )then
      begin
        box( row, col, 27, 8 );
        i := ( 30 - strlen( month_name[ month ] ) - 6 ) div 2;
        poscursor( row - 1, col + i );
        writeln( month_name[ month ], ' 19', year : 1 );
        poscursor( row + 1, col + 1 );
        writeln( 'Sun Mon Tue Wed Thu Fri Sat' );
      end
    else
      begin
        box( row, col, 20, 8 );
        i := ( 23 - strlen( month_name[ month ] ) - 5 ) div 2;
        poscursor( row - 1, col + i );
        writeln( month_name[ month ], ' 19', year : 1 );
        poscursor( row + 1, col + 1 );
        writeln( 'Su Mo Tu We Th Fr Sa' );
      end;
    if( ( year mod 4 = 0 ) and ( month = february ) )then
        days := 29
    else
        days := days_in_month[ month ];
    row := row + 3;
    poscursor( row, col + 1 );
    if( start <> 1 )then
        write( ' ' : wide * ( start - 2 ) + wide - 1 );
    for i := 1 to days do
      begin
        if( i = today )then
            foreground( yellow );
        if( start = 1 )then
            write( i : wide - 1 )
        else
            write( i : wide );
        if( start = 7 )then
          begin
            writeln;
            row := row + 1;
            poscursor( row, col + 1 );
          end;
        start := start mod 7 + 1;
        if( i = today )then
            foreground( cyan );
      end;
  end;

  begin
    page;
    foreground( cyan );
    datestr( today );
    year := ord( today[ 1 ] ) * 10 + ord( today[ 2 ] ) - 11 * ord( '0' );
    month := ord( today[ 4 ] ) * 10 + ord( today[ 5 ] ) - 11 * ord( '0' );
    day := ord( today[ 7 ] ) * 10 + ord( today[ 8 ] ) - 11 * ord( '0' );
    calendar( month, year, 10, 26, 4, day );
    month := month - 1;
    if( month = 0 )then
      begin
        month := 12;
        year := year - 1;
      end;
    calendar( month, year, 5, 3, 3, 0 );
    month := month + 2;
    if( month > 12 )then
      begin
        month := month mod 12;
        year := year + 1;
      end;
    calendar( month, year, 5, 56, 3, 0 );
    poscursor( 20, 1 );
  end.
