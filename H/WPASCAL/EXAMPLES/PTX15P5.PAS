{ Example 15.5 }
program main( output );
  var
    x, y, z, t : real;
  begin
    x := 3.1;
    y := 3.1e29;
    z := y - y + x;
    t := y + x - y;
    writeln( z, t );
  end.
