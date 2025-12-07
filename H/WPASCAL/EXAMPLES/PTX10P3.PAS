{ Example 10.3 }
program main( output );
  const
    gradecount = 5;
  var
    students : text;
    number : integer;
    age, aggregate : real;
    name : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
    marks : array[1..gradecount]of real;
    i : integer;
  begin
    reset( students, "student" );
    while( not eof( students ) )do
      begin
        read( students, number, filler, name, sex, age );
        for i := 1 to gradecount do
          read( students, marks[i] );
        readln( students );
        aggregate := 0;
        for i := 1 to gradecount do
          aggregate := aggregate + marks[i];
        writeln( number:4, " ", name:14,
          (aggregate/gradecount):6:2 );
      end;
  end.
