program ellipse_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the ELLIPSE procedure of the graphics library.
  The program draws two ellipses, each with different x and y axes.
}

  begin
    init( 0 );
    ellipse( 0.3, 0.5, 0.2, 0.1, 1 );
    ellipse( 0.7, 0.5, 0.1, 0.4, 2 );
    finish;
  end.
