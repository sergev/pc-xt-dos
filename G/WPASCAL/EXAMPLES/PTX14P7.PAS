{ Example 14.7 }
program main( input, output );
  type
    phone_number = packed array [ 1..8 ] of char;
  var
    name : packed array [ 1..20 ] of char;
    number : phone_number;
    file1 : file of phone_number;
  begin
    reset( file1, "numbers" );
    while( not eof( file1 ) )do
      begin
        read( file1, number );
        getkey( file1, name );
        writeln( name, ' ', number );
      end;
  end.
