{ Example 8.1 }
program main( output );
  var
    total, i : integer;
  begin
    total := 0;
    i := 1;
    while( i <= 100 )do
      begin
        total := total + i;
        i := i + 1;
      end;
    writeln( total );
  end.
