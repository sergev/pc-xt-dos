{
  This program creates a keyed file from the STUDENT file.
  Operations are then allowed on the file including
    - displaying the file
    - searching for a particular student
    - deleting a student from the file
    - adding a student from the file
}

program key_demo( input, output );

const
    NUM_MARKS = 5;

type
    string = packed array [ 1..14 ] of char;
    student_record = record
            number : integer;
            age : integer;
            sex : char;
            marks : array [ 1..NUM_MARKS ] of integer;
        end;

var
    key_file : file of student_record;
    student : student_record;
    name : string;
    choice : integer;

procedure zap_upper( var str : string );

  var
    i : integer;
    ch : char;

  begin
    for i := 1 to strlen( name ) do
      begin
        ch := name[ i ];
        if( ( ch >= 'a' ) and ( ch <= 'z' ) )then
            ch := chr( ord( ch ) + ord( 'A' ) - ord( 'a' ) );
        name[ i ] := ch;
      end;
  end;

procedure display_student( name : string; student : student_record );

  var
    sum, i : integer;

  begin
    write( name, ' ' : 14 - strlen( name ),
                 student.number, student.age, student.sex : 3 );
    sum := 0;
    for i := 1 to NUM_MARKS do
      begin
        write( student.marks[ i ] );
        sum := sum + student.marks[ i ];
      end;
    writeln( sum / NUM_MARKS : 9 : 1 );
  end;

procedure create_key_file;

  var
    f : text;
    filler : char;
    i : integer;

  begin
    createkey( key_file, 'stdkey' );
    reset( f, 'student' );
    while( not eof( f ) )do
      begin
        read( f, student.number, filler, name, student.sex, student.age );
        for i := 1 to NUM_MARKS do
            read( f, student.marks[ i ] );
        readln( f );
        rtrim( name );
        setnextkey( key_file, name );
        write( key_file, student );
      end;
  end;

procedure read_key_file;

  begin
    reset( key_file, 'stdkey' );
    while( not eof( key_file ) )do
      begin
        read( key_file, student );
        getkey( key_file, name );
        display_student( name, student );
      end;
  end;

procedure inquire_key_file;

  begin
    update( key_file, 'stdkey' );
    iocheck( FALSE );
    repeat
        prompt( 'Enter a name ', name );
        if( name <> "" )then
          begin
            zap_upper( name );
            setnextkey( key_file, name, TRUE );
            read( key_file, student );
            if( iostatus <> 0 )then
                writeln( 'Student does not exist' )
            else
                display_student( name, student );
          end;
    until( name = "" );
    iocheck( TRUE );
  end;

procedure remove_key_file;

  begin
    update( key_file, 'stdkey' );
    iocheck( FALSE );
    repeat
        prompt( 'Enter a name ', name );
        if( name <> "" )then
          begin
            zap_upper( name );
            setnextkey( key_file, name, TRUE );
            deletekey( key_file );
            if( iostatus <> 0 )then
                writeln( 'Student does not exist' )
            else
                writeln( 'Student successfully removed from keyed file' );
          end;
    until( name = "" );
  end;

 procedure add_to_key_file;

  begin
    update( key_file, 'stdkey' );
    writeln( 'Enter student information' );
    prompt( 'Name   ', name );
    prompt( 'Number ', student.number );
    prompt( 'Age    ', student.age );
    prompt( 'Sex    ', student.sex );
    prompt( 'Marks  ', student.marks[ 1 ], student.marks[ 2 ], student.marks[ 3 ],
                       student.marks[ 4 ], student.marks[ 5 ] );
    zap_upper( name );
    setnextkey( key_file, name, FALSE );
    iocheck( FALSE );
    write( key_file, student );
    if( iostatus <> 0 )then
        writeln( 'Could not add student to keyed file' )
    else
        writeln( 'Student successfully added to keyed file' );
  end;

  begin
    repeat
        page;
        writeln;
        writeln( '  1  -  create a keyed file' );
        writeln( '  2  -  read a keyed file' );
        writeln( '  3  -  search a keyed file' );
        writeln( '  4  -  remove a student' );
        writeln( '  5  -  add a student' );
        writeln( '  6  -  quit' );
        writeln;
        prompt( 'Enter your choice ', choice );
        writeln;
        case( choice )of
            1 : create_key_file;
            2 : read_key_file;
            3 : inquire_key_file;
            4 : remove_key_file;
            5 : add_to_key_file;
        else
            { do nothing }
        end;
        writeln;
        write( 'Press return' );
        readln;
    until( choice > 5 );
  end.
