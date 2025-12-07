{ Example 5.3 }
program main( input, output );
  var
    word : packed array[1..100]of char;
    newstr : packed array[1..200]of char;
    letter : packed array[1..1]of char;
    length, i : integer;
  begin
    writeln( "Please enter a word:" );
    readln( word );
    length := strlen( word );
    substr( word, 1, 1, newstr );
    i := 2;
    while( i <= length )do
      begin
        strconcat( newstr, " " );
        substr( word, 1, i, letter );
        strconcat( newstr, letter );
        i := i + 1;
      end;
    writeln( newstr );
  end.
