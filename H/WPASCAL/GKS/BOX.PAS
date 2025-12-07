program box_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the BOX procedure in the graphics library.
  The program draws three boxes, each in a different colour.
}

  begin
    init( 0 );
    box( 0.1, 0.9, 0.3, 0.7, 1 );
    box( 0.3, 0.5, 0.6, 0.0, 2 );
    box( 0.7, 0.7, 0.9, 0.6, 3 );
    finish;
  end.
