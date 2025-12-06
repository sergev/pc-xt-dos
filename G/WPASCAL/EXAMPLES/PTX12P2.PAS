{ Example 12.2 }
program main( output );
  var
    ch : char;
  begin
    for ch := 'a' to 'z' do
      writeln( ch, ord( ch ) );
  end.
