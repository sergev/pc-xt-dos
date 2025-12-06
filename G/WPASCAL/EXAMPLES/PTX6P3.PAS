{ Example 6.3 }
program main( input, output );
  var
    name : packed array[1..20]of char;
    file1 : text;
  begin
    rewrite( file1, "names" );
    writeln( "Enter name:" );
    readln( name );
    while( name <> "quit" )do
      begin
        writeln( file1, name );
        writeln( "Enter name:" );
        readln( name );
      end;
  end.
