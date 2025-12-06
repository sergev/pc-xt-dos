{ Example 10.7 }
program main( input, output );
  type
    namestr = packed array[1..20]of char;
  var
    names : array[1..5]of namestr;
    i : integer;
  begin
    for i := 1 to 5 do
      begin
        writeln( "Enter name:" );
        readln( names[i] );
      end;
    writeln( "The names in reverse order are:" );
    for i := 5 downto 1 do
      begin
        writeln( names[i] );
      end;
  end.
