{ Example 10.5 }
program main( output );
  var
    students : text;
    i, number, age : integer;
    name : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
    agecount : array[15..18]of integer;
  begin
    reset( students, "student" );
    for i := 15 to 18 do
      agecount[i] := 0;
    while( not eof( students ) )do
      begin
        readln( students,
          number, filler, name, sex, age );
        agecount[age] := agecount[age] + 1;
      end;
    writeln( "Age 15:":8, "Age 16:":8, "Age 17:":8, "Age 18:":8 );
    for i := 15 to 18 do
      write( agecount[i]:8 );
    writeln;
  end.
