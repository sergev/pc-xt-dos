{ Example 5.4 }
program main( input, output );
  var
    line, newline, word : packed array[1..80]of char;
    pos : integer;
  begin
    writeln( "Enter a line of text:" );
    readln( line );
    newline := "";
    pos := strscan( line, " " );
    while( pos <> 0 )do
      begin
        substr( line, pos, 1, word );
        strinsert( newline, word, 1 );
        strdelete( line, pos, 1 );
        pos := strscan( line, " " );
      end;
    strinsert( newline, " ", 1 );
    strinsert( newline, line, 1 );
    writeln( newline );
  end.
