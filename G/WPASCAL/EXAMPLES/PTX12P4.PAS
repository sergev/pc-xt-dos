{ Example 12.4 }
program main( input, output );
  var
    word : packed array[1..100]of char;
    ch : char;
     
  function isalpha( ch : char ) : boolean;
    begin
      isalpha := ( (ch >= 'a') and (ch <= 'z') ) or
                 ( (ch >= 'A') and (ch <= 'Z') );
    end;
     
  procedure getchar;
    begin
      read( ch );
    end;
     
  procedure readword;
    var
      i : integer;
    begin
      i := 1;
      while( not isalpha( ch ) )do
        getchar;
      while( isalpha( ch ) )do
        begin
          word[i] := ch;
          i := i + 1;
          getchar;
        end;
      word[i] := strend;
    end;
     
  begin
    writeln( "Enter some text:" );
    getchar;
    readword;
    while( word <> "quit" )do
      begin
        writeln( word );
        readword;
      end;
  end.
