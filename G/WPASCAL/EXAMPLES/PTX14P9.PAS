{ Example 14.9 }
program main( input, output );
  var
    filename, error : packed array [ 1..80 ] of char;
    file1 : text;
  begin
    writeln( "Enter a filename:" );
    readln( filename );
    iocheck( false );
    reset( file1, filename );
    if( iostatus <> 0 )then
      begin
        ioerrmsg( error );
        writeln( "The file could not be opened" );
        writeln( error );
      end
    else
        writeln( "The file exists" );
  end.
