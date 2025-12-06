program paint_boundary_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the PAINT and PAINT_BOUNDARY procedures in the
  graphics library. The program draws 2 boxes and paints each to show the
  difference between the two routines.
}

  begin
    init( 0 );
    block( 0.0, 0.0, 1.0, 0.5, 1 );
    box( 0.2, 0.2, 0.4, 0.8, 3 );
    box( 0.6, 0.2, 0.8, 0.8, 3 );
    paint( 0.3, 0.6, 2 );
    paint_boundary( 0.7, 0.6, 2, 3 );
    finish;
  end.
