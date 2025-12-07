{ Example 15.2 }
program main( input, output );
  var
    i : integer;
     
  function fact( n : integer ) : integer;
    begin
      if( n = 0 )then
        fact := 1
      else
        fact := n * fact( n-1 );
    end;
     
  begin
    writeln( "Enter a number:" );
    readln( i );
    while( i >= 0 )do
      begin
        writeln( i, fact( i ) );
        writeln( "Enter a number:" );
        readln( i );
      end;
  end.
