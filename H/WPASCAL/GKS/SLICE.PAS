program slice_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the SLICE procedure of the graphics library
  by drawing a simple pie graph.
}

  begin
    init( 0 );
    slice( 0.5, 0.5, 0.2, 1, 0, 90 );
    slice( 0.5, 0.5, 0.2, 2, 90, 290 );
    slice( 0.5, 0.5, 0.2, 3, 290, 360 );
    finish;
  end.
