{ Example 1.3 }
program main( output );
  var
    principal : real;
    interest : real;
    year : integer;
  begin
    principal := 100;
    year := 1;
    while( year <= 10 )do
      begin
        interest := principal * 0.12;
        writeln( principal, interest );
        principal := principal + interest;
        year := year + 1;
      end;
  end.
