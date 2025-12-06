  const
    PIC_SIZE    = 1000;         { this constant may be made larger }

  type
    picture = packed array [ 1..PIC_SIZE ] of char;


  procedure getpic( px, py, qx, qy : real; var pic : picture );

    const
      GETPIC    = -2;
      MAX_LEN   = 20;

    var
      error, across, down, len_data : integer;
      ints : array [ 1..3 ] of integer;
      reals : array [ 1..2 ] of real;
      data : packed array [ 1..MAX_LEN ] of char;

    begin
      gqpxad( WK_ID, px, py, qx, qy, error, across, down );
      ints[ 1 ] := WK_ID;
      ints[ 2 ] := across;
      ints[ 3 ] := down;
      if( px < qx )then
          reals[ 1 ] := px
      else
          reals[ 1 ] := qx;
      if( py > qy )then
          reals[ 2 ] := py
      else
          reals[ 2 ] := qy;
      gprec( 3, ints, 2, reals, "", MAX_LEN, error, len_data, data );
      gesc( GETPIC, len_data, data, PIC_SIZE, len_pic, pic );
    end;
