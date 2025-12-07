{ Example 2.4 }
program main( input, output );
  var
    fahrenheit : real;
    centigrade : real;
    kind : integer;
  begin
    writeln( "Enter 1 for centigrade to fahrenheit." );
    writeln( "Enter 2 for fahrenheit to centigrade." );
    readln( kind );
    if( kind = 1 )then
      begin
        writeln( "Enter degrees centigrade:" );
        readln( centigrade );
        fahrenheit := ( centigrade * 9 ) / 5 + 32;
        writeln( centigrade:8:2, fahrenheit:8:2 );
      end
    else
      begin
        writeln( "Enter degrees fahrenheit:" );
        readln( fahrenheit );
        centigrade := ( ( fahrenheit - 32 ) * 5 ) / 9;
        writeln( fahrenheit:8:2, centigrade:8:2 );
      end;
  end.
