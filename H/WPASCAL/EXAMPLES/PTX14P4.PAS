{ Example 14.4 }
program main( input, output );
     
  type personal_data =
    record
      name : packed array[1..40]of char;
      age : integer;
    end;
     
  var
    persons : file of personal_data;
    person : personal_data;
    record_number : integer;
     
  procedure read_record_number;
    begin
      writeln( "Enter record number:" );
      readln( record_number );
    end;
     
  begin
    update( persons, "pdata" );
    read_record_number;
    while( record_number >= 0 )do
      begin
        setnext( persons, record_number );
        read( persons, person );
        writeln( person.name, person.age );
        read_record_number;
      end;
  end.
