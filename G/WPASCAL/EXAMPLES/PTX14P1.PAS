{ Example 14.1 }
program main( output );
  type
    date =
      record
        day : 1..31;
        month : packed array[1..3]of char;
        year : integer;
      end;
  var
    today : date;
  begin
    today.day := 1;
    today.month := "Jan";
    today.year := 1985;
    writeln( today.month:4, today.day:3, today.year:5 );
  end.
