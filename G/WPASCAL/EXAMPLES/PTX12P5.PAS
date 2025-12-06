{ Example 12.5 }
program main( input, output );
  var
    value : integer;
    ch : char;
     
  procedure getchar;
    begin
      read( ch );
    end;
     
  procedure readnumber;
    begin
      value := 0;
      while( not( (ch >= '0') and (ch <= '9') ) )do
        getchar;
      while( (ch >= '0') and (ch <= '9') )do
        begin
          value := value * 10 + ord( ch ) - ord( '0' );
          getchar;
        end;
    end;
     
  begin
    writeln( "Enter a number:" );
    getchar;
    readnumber;
    writeln( value );
  end.
