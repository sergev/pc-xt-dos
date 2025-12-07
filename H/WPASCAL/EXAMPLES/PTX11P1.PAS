{ Example 11.1 }
program main( output );
  var
    students : text;
    number, i : integer;
    age, aggregate : real;
    name : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
    marks : array[1..5]of real;
     
  function average : real;
    begin
      aggregate := 0;
      i := 1;
      while( i <= 5 )do
        begin
          aggregate := aggregate + marks[i];
          i := i + 1;
        end;
      average := aggregate / 5;
    end;
     
  begin
    reset( students, "student" );
    while( not eof( students ) )do
      begin
        readln( students,
          number, filler, name, sex, age,
          marks[1], marks[2], marks[3], marks[4], marks[5] );
        writeln( number:4, " ", name:14, average:6:2 );
      end;
  end.
