{ Example 10.4 }
program main( input, output );
  var
    numbers : array[1..100]of real;
    number, aggregate, average : real;
    count, i : integer;
  begin
    count := 0;
    writeln( "Enter number:" );
    readln( number );
    while( number >= 0 )do
      begin
        count := count + 1;
        numbers[count] := number;
        writeln( "Enter number:" );
        readln( number );
      end;
    aggregate := 0;
    for i := 1 to count do
      aggregate := aggregate + numbers[i];
    average := aggregate / count;
    writeln( "The following are above average:" );
    for i := 1 to count do
      begin
        if( numbers[i] > average )then
          writeln( numbers[i]:10:5 );
      end;
  end.
