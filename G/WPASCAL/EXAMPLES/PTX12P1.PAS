{ Example 12.1 }
program main( output );
  var
    i : integer;
  begin
    for i := 0 to 255 do
      writeln( i, chr( i ) );
  end.
