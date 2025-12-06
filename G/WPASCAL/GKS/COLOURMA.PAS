program colourmap_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the COLOURMAP procedure of the graphics library.
  Each of the four available colours is associated with a different colour.
  The program then draws 3 blocks.
}

  begin
    init( 0 );
    colourmap( 0, BLUE );
    colourmap( 1, RED );
    colourmap( 2, GREEN );
    colourmap( 3, YELLOW );
    block( 0.1, 0.9, 0.3, 0.7, 1 );
    block( 0.3, 0.5, 0.6, 0.0, 2 );
    block( 0.7, 0.7, 0.9, 0.6, 3 );
    finish;
  end.
