{ Example 11.4 }
program main( output );
  var
    i : integer;
     
  function cube( x : integer ) : integer;
    begin
      cube := x * x * x;
    end;
     
  begin
    for i := 1 to 10 do
      writeln( i, sqr( i ), cube( i ) );
  end.
