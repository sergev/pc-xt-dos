program circular_arc_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the CIRC_ARC procedure of the graphics library.
  The program is similar to the CIRCLE example, except that only partial
  circles are drawn.
}

  begin
    init( 0 );
    circ_arc( 0.2, 0.7, 0.1, 1, 0, 180 );;
    circ_arc( 0.7, 0.5, 0.3, 2, 0, 180 );;
    finish;
  end.
