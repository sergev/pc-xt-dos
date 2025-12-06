{ Example 2.5 }
program main( output );
  var
    x : real;
    y : real;
    z : real;
  begin
    x := 1;
    while( x <= 10 )do
      begin
        y := sqr( x );
        z := sqrt( x );
        writeln( x:8:2, y:8:2, z:8:2 );
        x := x + 1;
      end;
  end.
