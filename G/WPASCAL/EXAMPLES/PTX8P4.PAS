{ Example 8.4 }
program main( output );
  var
    total, i : integer;
  begin
    total := 0;
    for i := 1 to 10 do
      total := total + i;
    writeln( total );
  end.
