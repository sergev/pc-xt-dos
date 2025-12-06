{ Example 14.2 }
program main( input, output );
     
  type personal_data =
    record
      name : packed array[ 1..40 ]of char;
      age : integer;
    end;
     
  var
    persons : file of personal_data;
    person : personal_data;
     
  procedure read_person;
    begin
      writeln( "Enter a name," );
      writeln( "or an empty line to quit:" );
      readln( person.name );
      if( person.name <> "" )then
        begin
         writeln( "Enter an age" );
         readln( person.age );
        end;
    end;
     
  begin
    rewrite( persons, "pdata" );
    read_person;
    while( person.name <> "" )do
      begin
        write( persons, person );
        read_person;
      end;
  end.
