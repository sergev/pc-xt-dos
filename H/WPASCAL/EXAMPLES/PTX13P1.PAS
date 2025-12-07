{ Example 13.1 }
program main( output );
  {
    This program reads the student file and
    displays a table of the total number of
    students for each sex and age category.
   }
  const
    min_age = 15;
    max_age = 18;
    name_length = 14;
  type
    sexes = ( male, female );
    ages = min_age..max_age;
  var
    students : text;
    count : array[ages,sexes]of integer;
     
  procedure initialize;
    var
      age : ages;
      sex : sexes;
    begin
      reset( students, "student" );
      for age := min_age to max_age do
        for sex := male to female do
          count[age, sex] := 0;
    end;
     
  procedure read_file;
    type
      namestr = packed array[1..name_length]of char;
      sexstr = packed array[1..1]of char;
      fillerstr = packed array[1..1]of char;
    var
      age : ages;
      number : integer;
      name : namestr;
      sex : sexstr;
      filler : fillerstr;
    begin
      while( not eof( students ) )do
        begin
          readln( students, number, filler, name, sex, age );
          if( sex = "F" )then
            count[age,female] := count[age,female] + 1
          else
            count[age,male] := count[age,male] + 1;
        end;
    end;
     
  procedure write_totals;
    const
      age_width = 3;
      total_width = 8;
    var
      age : ages;
    begin
      writeln( " ":age_width, " ":age_width,
        "Females":total_width, "Males":total_width );
      for age := min_age to max_age do
        writeln( "Age", age:age_width,
         count[age,female]:total_width, count[age,male]:total_width );
      writeln;
    end;
     
  begin
    initialize;
    read_file;
    write_totals;
  end.
