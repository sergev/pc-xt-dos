program main( input, output );

{ include the file GKSCOVER.PAS here }

procedure dog_demo;

  var
    m, x, y : integer;
    angle, start : real;
    pic1, pic2 : picture;

  procedure scroll( y : real );

    begin
        if( y > 1.0 )then
            y := 1.0;
        getpic( 0.6897, y, 1.0000, y - 0.2010, pic1 );
        getpic( 0.6897, y - 0.0251, 1.0000, y - 0.2010, pic2 );
        putpic_action( 0.6897, y, pic1, 1 );
        putpic( 0.6897, y, pic2 );
    end;

  begin
    { initialize }
    init( 0 );
    colourmap( 0, BLACK );
    colourmap( 1, LIGHT_GREEN );
    colourmap( 2, LIGHT_RED );
    colourmap( 3, LIGHT_YELLOW );

    { draw the grass }
    line( 0.0000, 0.0000, 1.0000, 0.0000, 1 );
    for m := 0 to 106 do
        line( m * 0.0094, 0.0201, m * 0.0094, 0.0050, 1 );

    { draw the tree }
    line( 0.1097, 0.0251, 0.1097, 0.3568, 3 );
    line( 0.1317, 0.0251, 0.1317, 0.3568, 3 );
    circle( 0.1254, 0.5477, 0.0313, 1 );
    paint( 0.1254, 0.5477, 1 );
    getpic( 0.0940, 0.5980, 0.1567, 0.4975, pic1 );
    putpic_action( 0.1097, 0.4472, pic1, 3 );
    putpic_action( 0.0784, 0.4472, pic1, 3 );
    putpic_action( 0.0313, 0.4975, pic1, 3 );
    putpic_action( 0.1411, 0.4975, pic1, 3 );
    putpic_action( 0.0784, 0.4975, pic1, 3 );
    putpic_action( 0.0313, 0.5477, pic1, 3 );
    putpic_action( 0.0940, 0.5477, pic1, 3 );
    putpic_action( 0.1567, 0.5477, pic1, 3 );
    putpic_action( 0.0470, 0.5980, pic1, 3 );
    putpic_action( 0.1411, 0.5980, pic1, 3 );
    putpic_action( 0.0940, 0.5980, pic1, 3 );
    putpic_action( 0.0784, 0.6482, pic1, 3 );
    putpic_action( 0.1097, 0.6482, pic1, 3 );

    { draw the sun }
    circle( 0.0940, 0.8492, 0.0470, 2 );
    paint( 0.0940, 0.8492, 3 );
    for m := 0 to 11 do
      begin
        angle := 2 * 3.141592654 * ( m / 12 );
        line( 0.0904 + 0.0627*cos( angle ), 0.8492 - 0.0854*sin( angle ),
              0.0904 + 0.0815*cos( angle ), 0.8492 - 0.1106*sin( angle ), 3 );
      end;

    { draw the dog }
    ellipse( 0.7837, 0.0955, 0.0564, 0.0402, 3 );
    ellipse( 0.7085, 0.1005, 0.0313, 0.0251, 3 );
    paint( 0.7837, 0.1206, 3 );
    paint( 0.7085, 0.1005, 3 );
    circle( 0.6991, 0.1106, 0.0063, 0 );
    circle( 0.6991, 0.1156, 0.0031, 0 );
    line( 0.6583, 0.0804, 0.7053, 0.0804, 0 );
    circle( 0.6740, 0.1005, 0.0031, 2 );
    ellipse( 0.7210, 0.0854, 0.0094, 0.0352, 2 );
    paint_boundary( 0.7210, 0.0854, 0, 2 );
    ellipse( 0.7210, 0.0854, 0.0094, 0.0352, 3 );
    block( 0.7586, 0.0955, 0.7618, 0.0251, 3 );
    block( 0.8056, 0.0955, 0.8088, 0.0251, 3 );
    line( 0.8025, 0.0251, 0.8119, 0.0251, 3 );
    line( 0.7555, 0.0251, 0.7649, 0.0251, 3 );
    ellipse( 0.7774, 0.1156, 0.0125, 0.0101, 0 );
    paint( 0.7774, 0.1156, 0 );
    ellipse( 0.7680, 0.0854, 0.0063, 0.0050, 0 );
    paint( 0.7680, 0.0854, 0 );
    ellipse( 0.8025, 0.0905, 0.0063, 0.0151, 0 );
    paint( 0.8025, 0.0905, 0 );
    line( 0.8307, 0.1055, 0.8558, 0.1307, 3 );
    line( 0.8307, 0.1005, 0.8558, 0.1307, 3 );
    getpic( 0.6583, 0.1357, 0.8809, 0.0241, pic1 );
    start := time;
    while( time < start + 1 )do
        ; { do nothing }

    { draw the doggy }
    block( 0.7586, 0.0955, 0.7618, 0.0251, 0 );
    block( 0.8056, 0.0955, 0.8088, 0.0251, 0 );
    line( 0.8025, 0.0251, 0.8119, 0.0251, 0 );
    line( 0.7555, 0.0251, 0.7649, 0.0251, 0 );
    line( 0.7586, 0.0955, 0.7492, 0.0251, 3 );
    line( 0.7618, 0.0955, 0.7524, 0.0251, 3 );
    line( 0.8056, 0.0955, 0.8213, 0.0251, 3 );
    line( 0.8088, 0.0955, 0.8245, 0.0251, 3 );
    line( 0.8307, 0.1055, 0.8558, 0.1307, 0 );
    line( 0.8307, 0.1005, 0.8558, 0.1307, 0 );
    line( 0.8307, 0.1005, 0.8495, 0.1357, 3 );
    line( 0.8307, 0.1055, 0.8495, 0.1357, 3 );
    getpic( 0.6583, 0.1357, 0.8809, 0.0251, pic2 );
    for m := 1 to 25 do
        if( m mod 2 = 0 )then
            putpic( 0.6583 - m * 0.0125, 0.1357, pic1 )
        else
            putpic( 0.6583 - m * 0.0125, 0.1357, pic2 );

    { draw the weight }
    line( 0.4483, 0.9497, 0.4859, 0.9497, 2 );
    line( 0.4859, 0.9497, 0.5799, 0.7990, 2 );
    line( 0.5799, 0.7990, 0.3542, 0.7990, 2 );
    line( 0.3542, 0.7990, 0.4483, 0.9497, 2 );
    box( 0.4608, 0.9497, 0.4734, 0.9648, 2 );
    paint( 0.4545, 0.9397, 2 );
    text( 0.4326, 0.8442, "5OO", 3 );
    text( 0.4013, 0.8040, "TONNES", 3 );

    { drop the weight }
    getpic( 0.3542, 1.0000, 0.5809, 0.7980, pic1 );
    text( 0.7022, 0.8040, "heh-heh-heh", 3 );
    for m := 0 to 26 do
        putpic( 0.3542, 1.0 - 0.0302 * m, pic1 );
    block( 0.7022, 0.8040, 1.0000, 0.8442, 0 );
    text( 0.7022, 0.8040, "Oh Dear!!! ", 3 );
    start := time;
    while( time < start + 2 )do
        ; { do nothing }

    { redraw the dog }
    ellipse( 0.3668, 0.0955, 0.0313, 0.0251, 3 );
    paint( 0.3699, 0.0955, 3 );
    circle( 0.3574, 0.1055, 0.0063, 0 );
    circle( 0.3574, 0.1106, 0.0031, 0 );
    line( 0.3166, 0.0754, 0.3636, 0.0754, 0 );
    circle( 0.3323, 0.0955, 0.0031, 2 );
    ellipse( 0.3793, 0.0804, 0.0094, 0.0352, 2 );
    paint_boundary( 0.3793, 0.0654, 0, 2 );
    ellipse( 0.3793, 0.0804, 0.0094, 0.0352, 3 );
    paint_boundary( 0.3793, 0.0654, 0, 3 );
    line( 0.5235, 0.1206, 0.5423, 0.1307, 3 );
    line( 0.5235, 0.1156, 0.5423, 0.1307, 3 );
    getpic( 0.5235, 0.1357, 0.5455, 0.1206, pic1 );
    line( 0.5235, 0.1206, 0.5423, 0.1307, 0 );
    line( 0.5235, 0.1156, 0.5423, 0.1307, 0 );
    line( 0.5235, 0.1206, 0.5361, 0.1357, 3 );
    line( 0.5235, 0.1156, 0.5361, 0.1357, 3 );
    getpic( 0.5235, 0.1357, 0.5455, 0.1206, pic2 );
    block( 0.3009, 0.8040, 1.0000, 0.8442, 0 );
    text( 0.3009, 0.8040, "Gee Wally, that was close!!!", 3 );

    { make the dog smile and wag tail }
    block( 0.3135, 0.1357, 0.3975, 0.0452, 0 );
    line( 0.3542, 0.0138, 0.4483, 0.1645, 2 );
    paint( 0.3950, 0.0600, 2 );
    ellipse( 0.3668, 0.0955, 0.0313, 0.0251, 3 );
    paint( 0.3699, 0.0905, 3 );
    ellipse( 0.3354, 0.0754, 0.0063, 0.0201, 3 );
    circle( 0.3574, 0.1055, 0.0031, 0 );
    circle( 0.3762, 0.1055, 0.0031, 0 );
    ell_arc( 0.3668, 0.0955, 0.0157, 0.0201, 0, 220, 330 );
    ellipse( 0.3981, 0.0754, 0.0063, 0.0201, 3 );
    paint_boundary( 0.3981, 0.0754, 0, 3 );
    circle( 0.3668, 0.0955, 0.0063, 2 );
    paint( 0.3668, 0.0955, 2 );
    for m := 1 to 50 do
      begin
        putpic( 0.5235, 0.1357, pic1 );
        for x := 1 to 20 do
            ; { do nothing }
        putpic( 0.5235, 0.1357, pic2 );
        for y := 1 to 20 do
            ; { do nothing }
      end;

    { credits }
    block( 0.3009, 0.8040, 1.0000, 0.8442, 0 );
    text( 0.3009, 0.8040, "THE END", 3 );
    text( 0.7022, 0.5226, "  ""dog"" by  ", 3 );
    scroll( 0.6030 );
    scroll( 0.6281 );
    text( 0.7022, 0.5226, " WATCOM and ", 3 );
    scroll( 0.6533 );
    scroll( 0.6784 );
    text( 0.7022, 0.5226, "K-9 softwear", 3 );
    for m := 1 to 20 do
        scroll( 0.6784 + 0.0251 * m );
    finish;
  end;

begin
    dog_demo;
end.
