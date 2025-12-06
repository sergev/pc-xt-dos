{ Example 15.1 }
program main( output );
  var
    i, j : integer;
     
  procedure increment( var x : integer );
    begin
      x := x + 1;
    end;
     
  begin
    i := 1;
    while( i <= 10 )do
      begin
        writeln( i );
        increment( i );
      end;
  end.
