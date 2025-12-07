program circle_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the CIRCLE procedure of the graphics library.
  The program draws two circles, each with a different radius.
}

  begin
    init( 0 );
    circle( 0.2, 0.7, 0.1, 1 );;
    circle( 0.7, 0.5, 0.3, 2 );;
    finish;
  end.
