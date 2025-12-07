{ Example 7.2 }
program main( output );
  var
    students : text;
    number, age, algebra : real;
    name : packed array[1..14]of char;
    sex, filler : packed array[1..1]of char;
    female_total, number_of_females, female_average : real;
    male_total, number_of_males, male_average : real;
  begin
    reset( students, "student" );
    female_total := 0;
    male_total := 0;
    number_of_females := 0;
    number_of_males := 0;
    while( not eof( students ) )do
      begin
        readln( students,
          number, filler, name, sex, age, algebra );
        if( sex = "F" )then
          begin
            female_total := female_total + algebra;
            number_of_females := number_of_females + 1;
          end
        else
          begin
            male_total := male_total + algebra;
            number_of_males := number_of_males + 1;
          end;
      end;
    female_average := female_total / number_of_females;
    male_average := male_total / number_of_males;
    writeln( "Female average is ", female_average:6:2 );
    writeln( "Male average is ", male_average:6:2 );
  end.
