{ Example 12.3 }
program main( input, output );
  var
    ch : char;
    frequency : array['a'..'z']of integer;
  begin
    writeln( "Enter some text:" );
    for ch := 'a' to 'z' do
      frequency[ch] := 0;
    while( not eoln )do
      begin
        read( ch );
        if( (ch >= 'a') and (ch <= 'z') )then
          frequency[ch] := frequency[ch] + 1;
      end;
    for ch := 'a' to 'z' do
      if( frequency[ch] <> 0 )then
        writeln( ch, ":", frequency[ch] );
  end.
