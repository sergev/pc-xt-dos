{ Example 7.3 }
program main( output );
  var
    students : text;
    number, age : integer;
    name : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
    total_15, total_16, total_17, total_18 : integer;
  begin
    reset( students, "student" );
    total_15 := 0;
    total_16 := 0;
    total_17 := 0;
    total_18 := 0;
    while( not eof( students ) )do
      begin
        readln( students,
          number, filler, name, sex, age );
        if( age = 15 )then
          total_15 := total_15 + 1
        else if( age = 16 )then
          total_16 := total_16 + 1
        else if( age = 17 )then
          total_17 := total_17 + 1
        else if( age = 18 )then
          total_18 := total_18 + 1
        else
          begin
            writeln( "Bad record." );
            writeln( number, " ", name, " ", sex, " ", age );
          end;
      end;
    writeln( "Age 15:":8, "Age 16:":8, "Age 17:":8, "Age 18:":8 );
    writeln( total_15:8, total_16:8, total_17:8, total_18:8 );
  end.
