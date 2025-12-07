{ Example 8.3 }
program main( output );
  var
    total, i : integer;
  begin
    total := 0;
    i := 1;
    repeat
      total := total + i;
      i := i + 1;
    until( i > 100 );
    writeln( total );
  end.
