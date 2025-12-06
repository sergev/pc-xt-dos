program elliptical_arc_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the ELL_ARC procedure of the graphics
  library. The program is similar to the ELLIPSE example, except that
  only partial ellipses are drawn.
}

  begin
    init( 0 );
    ell_arc( 0.3, 0.5, 0.2, 0.1, 1, 270, 90 );
    ell_arc( 0.7, 0.5, 0.1, 0.4, 2, 180, 360 );
    finish;
  end.
