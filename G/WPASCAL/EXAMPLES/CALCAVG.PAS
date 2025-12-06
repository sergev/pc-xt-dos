{
  This program reads the STUDENT file and displays
  the names of all the students who are above average.
  The student information is stored in a linked-list.
}

program average( output );

const
    NUM_MARKS = 5;

type
    string = packed array [ 1..14 ] of char;
    student_rec_ptr = @ student_record;
    student_record = record
        name : string;
        average : real;
        link : student_rec_ptr;
    end;

var
   list_head, list_tail : student_rec_ptr;
   stdfile : text;
   class_average : real;

procedure initialize;

  begin
    reset( stdfile, 'student' );
    list_head := nil;
  end;

procedure add_student;

  var
    current : student_rec_ptr;
    number, age, i, mark, sum : integer;
    filler, sex : char;

  begin
    new( current );
    read( stdfile, number, filler, current@.name, sex, age );
    sum := 0;
    for i := 1 to NUM_MARKS do
      begin
        read( stdfile, mark );
        sum := sum + mark;
      end;
    readln( stdfile );
    current@.average := sum / NUM_MARKS;
    current@.link := nil;
    { writeln( current@.name, current@.average : 2 : 1 ); }
    if( list_head = nil )then
      begin
        list_head := current;
        list_tail := current;
      end
    else
      begin
        list_tail@.link := current;
        list_tail := current;
      end;
  end;

procedure calc_average;

  var
    current : student_rec_ptr;
    count : integer;
    sum : real;

  begin
    sum := 0;
    count := 0;
    current := list_head;
    while( current <> nil )do
      begin
        sum := sum + current@.average;
        count := count + 1;
        current := current@.link;
      end;
    class_average := sum / count;
  end;

procedure display_elite;

  var
    current, next : student_rec_ptr;

  begin
    writeln;
    writeln( 'Class average is ', class_average : 2 : 1 );
    writeln;
    writeln( 'Students above class average are:' );
    writeln;
    current := list_head;
    while( current <> nil )do
      begin
        if( current@.average >= class_average )then
            writeln( current@.name, current@.average : 2 : 1 );
        next := current@.link;
        dispose( current );
        current := next;
      end;
  end;

  begin
    initialize;
    while( not eof( stdfile ) )do
        add_student;
    calc_average;
    display_elite;
  end.
