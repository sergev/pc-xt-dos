{ Example 1.2 }
program main( output );
  var
    principal : real;
    interest : real;
  begin
    principal := 100;
    interest := principal * 0.12;
    writeln( principal, interest );
  end.
