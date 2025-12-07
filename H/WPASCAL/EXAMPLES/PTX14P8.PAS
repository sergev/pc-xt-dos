{ Example 14.8 }
program main( input, output );
  type
    phone_number = packed array [ 1..8 ] of char;
  var
    name : packed array [ 1..20 ] of char;
    number : phone_number;
    file1 : file of phone_number;
  begin
    reset( file1, "numbers" );
    iocheck( false );
    writeln( "Enter name:" );
    readln( name );
    while( name <> "quit" )do
      begin
        setnextkey( file1, name );
        read( file1, number );
        if( iostatus <> 0 )then
            writeln( "Number not found" )
        else
            writeln( number );
        writeln( "Enter name:" );
        readln( name );
      end;
  end.
