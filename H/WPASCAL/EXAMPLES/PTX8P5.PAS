{ Example 8.5 }
program main( output );
  var
    total, i : integer;
  begin
    total := 0;
    for i := 1 downto -99 do
      total := total + i;
    writeln( total );
  end.
