{ Example 5.1 }
program main( input, output );
  var
    surname : packed array[1..30]of char;
    first_name : packed array[1..30]of char;
    initial : packed array[1..30]of char;
    full_name : packed array[1..90]of char;
  begin
    writeln( "What is your surname?" );
    readln( surname );
    writeln( "What is your first name?" );
    readln( first_name );
    writeln( "What is your middle initial?" );
    readln( initial );
    full_name := first_name;
    strconcat( full_name, " " );
    strconcat( full_name, initial );
    strconcat( full_name, ". " );
    strconcat( full_name, surname );
    writeln( full_name );
  end.
