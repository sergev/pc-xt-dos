{ Example 4.1 }
program main( output );
  var
    balance, principal, interest : real;
    month : integer;
  begin
    balance := 10000;
    month := 1;
    writeln( "Month":6, "Balance":9, "Interest":9, "Principal":10 );
    interest := balance * 0.01;
    principal := 750.00 - interest;
    while( balance > principal )do
      begin
        writeln( month:6, balance:9:2, interest:9:2, principal:10:2 );
        balance := balance - principal;
        interest := balance * 0.01;
        principal := 750.00 - interest;
        month := month + 1;
      end;
    writeln( month:6, balance:9:2, interest:9:2, balance:10:2 );
    writeln;
    writeln( "Number of months to repay is", month:6 );
  end.
