{ Example 6.4 }
program main( input, output );
  var
    name : packed array[1..20]of char;
    number : packed array[1..8]of char;
    file1 : text;
    blanks : integer;
  begin
    rewrite( file1, "telenums" );
    writeln( "Enter name:" );
    readln( name );
    while( name <> 'quit' )do
      begin
        writeln( "Enter number:" );
        readln( number );
        blanks := 20 - strlen( name );
        writeln( file1, name, " ":blanks, " ", number );
        writeln( "Enter name:" );
        readln( name );
      end;
  end.
