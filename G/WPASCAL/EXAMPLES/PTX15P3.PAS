{ Example 15.3 }
program main( input, output );
  var
    numbers : array[1..100]of real;
    number, average : real;
    count : integer;
     
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
    var
      aggregate : real;
     
    procedure calculate_aggregate;
      var
        i : integer;
      begin
       aggregate := 0;
       for i := 1 to count do
         aggregate := aggregate + numbers[i];
      end;
     
    begin
      calculate_aggregate;
      average := aggregate / count;
    end;
     
  procedure display_numbers;
    var
      i : integer;
    begin
      writeln( "The following are above average:" );
      for i := 1 to count do
        begin
          if( numbers[i] > average )then
            writeln( numbers[i]:10:5 );
        end;
    end;
     
  begin
    read_numbers;
    calculate_average;
    display_numbers;
  end.
