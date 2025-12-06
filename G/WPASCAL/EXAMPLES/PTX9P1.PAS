{ Example 9.1 }
program main( input, output );
  var
    file1 : text;
    number, age : integer;
    found : boolean;
    name, key : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
  begin
    writeln( "Enter name of student:" );
    readln( key );
    strconcat( key, "              " );
    reset( file1, "student" );
    found := false;
    while( not eof( file1 ) and not found )do
      begin
        readln( file1, number, filler, name, sex, age );
        found := (name = key);
      end;
    if( found )then
      writeln( name, sex, age )
    else
      writeln( "Not found." );
  end.
