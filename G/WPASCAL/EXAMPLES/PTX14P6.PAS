{ Example 14.6 }
program main( input, output );
  type
    phone_number = packed array [ 1..8 ] of char;
  var
    name : packed array [ 1..20 ] of char;
    number : phone_number;
    file1 : file of phone_number;
  begin
    createkey( file1, "numbers" );
    writeln( "Enter name:" );
    readln( name );
    while( name <> "quit" )do
      begin
        writeln( "Enter number:" );
        readln( number );
        setnextkey( file1, name );
        write( file1, number );
        writeln( "Enter name:" );
        readln( name );
      end;
  end.
