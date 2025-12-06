{ Example 6.7 }
program main( input, output );
  var
    line : packed array[1..128]of char;
    i : integer;
  begin
    reset( input, "testdata" );
    readln( line );
    while( line <> "" )do
      begin
        writeln( "The line is:", line );
        readln( line );
      end;
  end.
