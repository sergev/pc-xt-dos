{ Example 2.1 }
program main( output );
  var
    principal : real;
    interest : real;
    year : integer;
  begin
    writeln( "Principal", " Interest" );
    principal := 100;
    for year := 1 to 10 do
      begin
        interest := principal * 0.12;
        writeln( principal:8:2, interest:8:2 );
        principal := principal + interest;
      end;
  end.
