{ Example 1.4 }
program main( output );
  var
    principal : real;
    interest : real;
    year : integer;
  begin
    principal := 100;
    for year := 1 to 10 do
      begin
        interest := principal * 0.12;
        writeln( principal, interest );
        principal := principal + interest;
      end;
  end.
