{ Example 10.2 }
program main( input, output );
  var
    numbers : array[1..4]of integer;
    i : integer;
  begin
    for i := 1 to 4 do
      begin
        writeln( "Enter number:" );
        readln( numbers[i] );
      end;
    writeln( "The numbers in reverse order are:" );
    for i := 4 downto 1 do
      writeln( numbers[i] );
  end.
