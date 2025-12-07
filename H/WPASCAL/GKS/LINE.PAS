program line_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the LINE procedure of the graphics library.
  The program draws three lines, each in a different colour.
}

  begin
    init( 0 );
    line( 0.0, 0.0, 1.0, 1.0, 1 );
    line( 0.0, 1.0, 1.0, 0.0, 2 );
    line( 0.0, 0.5, 1.0, 0.5, 3 );
    finish;
  end.
