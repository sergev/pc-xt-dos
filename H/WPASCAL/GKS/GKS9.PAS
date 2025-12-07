program main( output );

  const
    WK_ID       = 14;
    MED_RES     = 2;
    ACROSS      = 3;
    DOWN        = 5;
    ULX         = 0.0;
    ULY         = 1.0;
    LRX         = 1.0;
    LRY         = 0.0;

  type
    cell_array = array [ 1..DOWN ] of array [ 1..ACROSS ] of integer;

  const
    cell = cell_array [ [ 1, 0, 0 ],
                        [ 1, 0, 0 ],
                        [ 1, 1, 0 ],
                        [ 1, 0, 1 ],
                        [ 1, 0, 1 ] ];

  begin
    gopks( 0 );
    gopwk( WK_ID, 0, MED_RES );
    gacwk( WK_ID );

    gca( ULX, ULY, LRX, LRY, 1, 1, ACROSS, DOWN, cell );

    gdawk( WK_ID );
    gclwk( WK_ID );
    gclks;
  end.
