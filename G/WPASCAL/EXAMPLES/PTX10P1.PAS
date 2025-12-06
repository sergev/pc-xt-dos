{ Example 10.1 }
program main( input, output );
  var
    numbers : array[1..4]of integer;
  begin
    writeln( "Enter first number:" );
    readln( numbers[1] );
    writeln( "Enter second number:" );
    readln( numbers[2] );
    writeln( "Enter third number:" );
    readln( numbers[3] );
    writeln( "Enter fourth number:" );
    readln( numbers[4] );
    writeln( "The numbers in reverse order are:" );
    writeln( numbers[4] );
    writeln( numbers[3] );
    writeln( numbers[2] );
    writeln( numbers[1] );
  end.
