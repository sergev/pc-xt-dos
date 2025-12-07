{ Example 5.2 }
program main( input, output );
  var
    word : packed array[1..3]of char;
    letter1, letter2, letter3 : packed array[1..1]of char;
    newstr : packed array[1..5]of char;
  begin
    writeln( "Enter a 3-letter word:" );
    readln( word );
    substr( word, 1, 1, letter1 );
    substr( word, 1, 2, letter2 );
    substr( word, 1, 3, letter3 );
    newstr := "";
    strconcat( newstr, letter1 );
    strconcat( newstr, " " );
    strconcat( newstr, letter2 );
    strconcat( newstr, " " );
    strconcat( newstr, letter3 );
    writeln( newstr );
  end.
