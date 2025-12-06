{ Example 10.6 }
program main( output );
  const
    female = 1;
    male = 2;
  var
    students : text;
    i, j, number, age : integer;
    name : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
    count : array[15..18,female..male]of integer;
  begin
    reset( students, "student" );
    for i := 15 to 18 do
      for j := female to male do
        count[i, j] := 0;
    while( not eof( students ) )do
      begin
        readln( students,
          number, filler, name, sex, age );
        if( sex = "F" )then
          count[age,female] := count[age,female] + 1
        else
          count[age,male] := count[age,male] + 1;
      end;
    writeln( " ":3, " ":3, "Females":8, "Males":8 );
    for i := 15 to 18 do
      writeln( "Age", i:3, count[i,female]:8, count[i,male]:8 );
    writeln;
  end.
