{ Example 6.2 }
program main( output );
  var
    file1 : text;
    number, age : integer;
    algebra, geometry, english : real;
    physics, chemistry : real;
    name : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
  begin
    reset( file1, "student" );
    while( not eof( file1 ) )do
      begin
        readln( file1,
          number, filler, name, sex, age, algebra,
          geometry, english, physics, chemistry );
        writeln( name );
      end;
  end.
