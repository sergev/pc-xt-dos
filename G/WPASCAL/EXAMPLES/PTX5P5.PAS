{ Example 5.5 }
program main( input, output );
  type
    namestr = packed array[1..30]of char;
    entire_namestr = packed array[1..90]of char;
  var
    surname, first_name, initial : namestr;
    full_name : entire_namestr;
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