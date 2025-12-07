{ Example 2.2 }
program main( input, output );
  var
    principal : real;
    interest : real;
    year : integer;
  begin
    writeln( "Enter principal:" );
    readln( principal );
    writeln( "Principal", " Interest" );
    for year := 1 to 10 do
      begin
        interest := principal * 0.12;
        writeln( principal:8:2, interest:8:2 );
        principal := principal + interest;
      end;
  end.
