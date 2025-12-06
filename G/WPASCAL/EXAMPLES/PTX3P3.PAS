{ Example 3.3 }
program main( output );
  var
    i : integer;
    total : integer;
  begin
    for i := 1 to 10 do
      begin
        total := total + sqr( i );
      end;
    writeln( total );
  end.
