{ Example 15.4 }
program main( output );
  var
    x : real;
  begin
    x := 1;
    while( x <> 2 )do
      begin
        writeln( x, sqrt( x ) );
        x := x + 0.1;
      end;
  end.
