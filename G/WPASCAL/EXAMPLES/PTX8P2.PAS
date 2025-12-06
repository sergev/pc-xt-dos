{ Example 8.2 }
program main( input, output );
  var
    total, i, x : integer;
  begin
    writeln( "Enter a number:" );
    readln( x );
    while( x >= 0 )do
      begin
        total := 0;
        i := 1;
        while( i <= x )do
          begin
            total := total + i;
            i := i + 1;
          end;
        writeln( total );
        writeln( "Enter a number:" );
        readln( x );
      end;
  end.
