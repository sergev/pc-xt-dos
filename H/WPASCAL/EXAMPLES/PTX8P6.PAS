{ Example 8.6 }
program main( output );
  var
    total, i, j : integer;
  begin
    for j := 5 to 20 do
      begin
        total := 0;
        for i := 1 to j do
          total := total + i;
        writeln( j, total );
      end;
  end.
