program text_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the TEXT procedure of the graphics library.
  The program displays 3 text strings, each in a different colour.
}

  begin
    init( 0 );
    text( 0.0, 0.0, "lower left corner", 1 );
    text( 0.4, 0.9, "centre top", 2 );
    text( 0.7, 0.5, "right edge", 3 );
    finish;
  end.
