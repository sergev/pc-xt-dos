{ Example 11.2 }
program main( input, output );
  var
    numbers : array[1..100]of real;
    number, aggregate, average : real;
    count, i : integer;
     
  procedure read_numbers;
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
    end;
     
  procedure calculate_average;
    begin
      aggregate := 0;
      i := 1;
      while( i <= count )do
        begin
          aggregate := aggregate + numbers[i];
          i := i + 1;
        end;
      average := aggregate / count;
    end;
     
  procedure display_numbers;
    begin
      writeln( "The following are above average:" );
      i := 1;
      while( i <= count )do
        begin
          if( numbers[i] > average )then
           writeln( numbers[i]:10:5 );
          i := i + 1;
        end;
    end;
     
  begin
    read_numbers;
    calculate_average;
    display_numbers;
  end.
