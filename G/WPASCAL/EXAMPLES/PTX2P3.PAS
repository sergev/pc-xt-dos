{ Example 2.3 }
program main( input, output );
  var
    first : integer;
    second : integer;
  begin
    writeln( "Enter the first number:" );
    readln( first );
    writeln( "Enter the second number:" );
    readln( second );
    if( first > second )then
      begin
        writeln( first, " is greater than", second );
      end
    else
      begin
        writeln( first, " is less than or equal to", second );
      end;
  end.
