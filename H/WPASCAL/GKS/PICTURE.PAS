program pic_example( input, output );

{ include the file GKSCOVER.PAS here }

{
  This program demonstrates the GETPIC, PUTPIC and PUTPIC_ACTION procedures
  of the graphics library.
}

procedure picture_example;

  var
    pic : picture;
    y : real;

  begin
    init( 0 );
    block( 0.1, 0.9, 0.3, 0.6, 1 );
    { take a picture of the block just drawn }
    getpic( 0.1, 0.9, 0.3, 0.6, pic );
    { redraw it in the centre of the screen }
    putpic( 0.4, 0.6, pic );
    { redraw it on the right side }
    putpic( 0.7, 0.9, pic );
    y := 0.9;
    while( y > 0.3 )do
      begin
        { use exclusive-or to erase old picture }
        putpic_action( 0.7, y, pic, 1 );
        { redraw it lower on the screen }
        y := y - 0.05;
        putpic( 0.7, y, pic );
      end;
    finish;
  end;

  begin
    picture_example;
  end.
