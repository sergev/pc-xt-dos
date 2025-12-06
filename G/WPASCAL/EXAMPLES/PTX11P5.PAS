{ Example 11.5 }
program main( input, output );
  type
    string100 = packed array[1..100]of char;
  var
    str : string100;
    count : integer;
     
  procedure strrpt( str1 : string100; count : integer );
    var
      i : integer;
      temp : string100;
    begin
      temp := "";
      for i := 1 to count do
        strconcat( temp, str1 );
      writeln( temp );
    end;
     
  begin
    writeln( "Enter a string:" );
    readln( str );
    while( str <> "quit" )do
      begin
        writeln( "Enter repetition count:" );
        readln( count );
        strrpt( str, count );
        writeln( "Enter a string:" );
        readln( str );
      end;
  end.
