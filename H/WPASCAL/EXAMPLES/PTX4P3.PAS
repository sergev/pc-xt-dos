{ Example 4.3 }
program main( output );
  const
    seed = 357;
    range = 6;
  var
    i : integer;
  begin
    initrandom( seed, range );
    for i := 1 to 10 do
      begin
        writeln( random+1, random+1 );
      end;
  end.
