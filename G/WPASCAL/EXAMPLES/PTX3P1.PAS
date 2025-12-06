{ Example 3.1 }
program main( output );
  var
    i : integer;
    total : integer;
  begin
    i := 1;
    total := 0;
    while( i <> 100 )do
      begin
        total := total + i;
        i := i + 2;
      end;
    writeln( total );
  end.
