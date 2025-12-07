{ Example 14.3 }
program main( input, output );
     
  type personal_data =
    record
      name : packed array[1..40]of char;
      age : integer;
    end;
     
  var
    persons : file of personal_data;
    person : personal_data;
     
  begin
    reset( persons, "pdata" );
    while( not eof( persons ) )do
      begin
        read( persons, person );
        writeln( "Name: ", person.name, "; age: ", person.age );
      end;
  end.
