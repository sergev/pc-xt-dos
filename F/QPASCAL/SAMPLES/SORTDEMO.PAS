PROGRAM sort_demo;
{===============================================================

    QuickPascal Sorting Demonstration
    ---------------------------------
    This program graphically demonstrates six common sorting
    algorithms.  It prints horizontal bars, all of different
    lengths and in random order, then sorts the bars from
    smallest to longest.

    The program also uses sound statements to generate
    different pitches, depending on the location of the bar
    being printed.  Note that the sound statements delay the
    speed of each sorting algorithm so you can follow the
    progress of the sort.  Therefore, the times shown are for
    comparison only.  They are not an accurate measure of sort
    speed.

    NOTE: If you press CTRL+BREAK while doing a sort in the
    QuickPascal environment, the speaker may be left on.
    To turn off the speaker, continue sort_demo (press F5), or
    step to a call to NoSound (press F10 repeatedly).

    If you use these sorting routines in your own programs, you
    may notice a difference in their relative speeds (for
    example, the exchange sort may be faster than the shell
    sort) depending on the number of elements to be sorted and
    how "scrambled" they are to begin with.

}
{$M 10000, 0, 0 }  { Heap is not used }
{$B-}              { Needs short-circuit Boolean evaluation}
{$R-}              { Range-checking off }

USES
    Crt, Dos;

CONST

    block      = #223;
    esc        = #27;
    null_char  = #0;
    space      = #32;

    ticks_per_sec = 18.2;  { Clock ticks per second }

    maxmax_bars = 43; { Absolute maximum number of bars }
    max_sorts   =  6; { Number of sorting algorithms }

    { Menu dimensions }
    menu_top    =  1;
    menu_left   = 49;
    menu_height = 18;
    menu_width  = 80 - menu_left;

    { Default colors, changed for monochrome displays }
    screen_back    : Byte = Black;
    menu_back      : Byte = LightGray;
    menu_frame     : Byte = LightBlue;
    menu_text      : Byte = Black;
    menu_status    : Byte = LightCyan;
    menu_highlight : Byte = Yellow;

TYPE
    sorts       = ( insertion, bubble, heap, exchange, shell, quick );
    sort_range  = First( sorts )..Last( sorts );
    sort_elements = RECORD
        len    : Byte;     { Bar length (the sort element) }
        color  : Integer;  { Bar color }
        END;
    sort_arrays = ARRAY[1..maxmax_bars] OF sort_elements;

VAR
    { Array of sorting procedures }
    sort : ARRAY[sort_range] OF PROCEDURE;
    sort_array,               { Elements to be sorted }
    unsorted_array            { Unsorted copy of array }
        : sort_arrays;
    max_bars,                 { Max bars to sort }
    max_colors  : Integer;    { Max colors supported }
    start_time,               { Starting time }
    finish_time : LongInt;    { Ending time }
    pause       : Word;       { Pause length }
    start_mode  : Word;       { Starting videomode }
    sound_on    : Boolean;    { True if sound enabled }
    bar         : STRING[maxmax_bars]; { String of bar characters }

    { Sound frequency scaling factor for the number of sort rows }
    scale_frequency : Integer;

CONST
    { List of menu items }
    menu : ARRAY[1..menu_height] OF CSTRING[30] =
        ( '  QuickPascal Sorting Demo',
          ' ',
          'Insertion',
          'Bubble',
          'Heap',
          'Exchange',
          'Shell',
          'Quick',
          ' ',
          'Toggle Sound: ',
          ' ',
          'Pause Factor: ',
          '<   (Slower)',
          '>   (Faster)',
          ' ',
          'Type first character of',
          'choice ( IBHESQT<> )',
          'or ESC key to end program: '
         );

{======================= elapsed_time ==========================
    Prints seconds elapsed since the sorting routine started.
    This time includes both the time it takes to redraw the bars
    and the pause while the sound statement plays a note, and thus
    is not an accurate indication of sorting speed.
}
PROCEDURE elapsed_time( current_row  : Integer;
                        current_sort : sort_range );

    BEGIN

    { Read clock ticks from predefined memory array. }
    finish_time := MemL[$40:$6C];

    TextColor( menu_status );
    TextBackground( menu_back );
    GotoXY( menu_left + 21, Ord(current_sort) + menu_top + 3 );
    Write( ((finish_time - start_time) / ticks_per_sec ):7:2 );

    IF sound_on THEN
        BEGIN
        Sound( current_row * scale_frequency );
        Delay( pause );
        NoSound;
        END
    ELSE
        Delay( pause );

    TextBackground( screen_back );
    END;  { elapsed_time }

{======================= swap_sort_elements ====================
    Exchanges two bars.
}
PROCEDURE swap_sort_elements( VAR one, two : sort_elements );
    VAR
        temp : sort_elements; { Holder for swap }
    BEGIN
    temp := one;
    one  := two;
    two  := temp;
    END; { swap_sort_elements }

{======================= print_one_bar ===========================
    Prints a bar at the row indicated by the row parameter.
}
PROCEDURE print_one_bar( row : Integer );
    VAR
        bar_end : Integer;
    BEGIN
    TextColor( sort_array[row].color );

    bar_end := sort_array[row].len;
    FillChar( bar[1],           bar_end,            block );
    FillChar( bar[bar_end + 1], max_bars - bar_end, space );
    bar[0] := Chr( max_bars );

    GotoXY( 1, row );
    Write( bar );
    END; { print_one_bar }

{======================= draw_bar_swap =========================
    Calls print_one_bar twice to switch the two bars in row1 and
    row2.
}
PROCEDURE draw_bar_swap( row1, row2 : Integer );
    BEGIN
    print_one_bar( row1 );
    print_one_bar( row2 );
    END; { draw_bar_swap }

{======================= rand_int ==============================
    Returns a random integer greater than or equal to the lower
    parameter and less than or equal to the upper parameter.
}
FUNCTION rand_int( lower, upper : Integer ) : Integer;
    BEGIN
    rand_int := Random( upper - lower ) + lower;
    END; { rand_int }


{$F+} { Enable far calls for sort procedures. }

{ ================== bubble_sort ===============================
    The bubble_sort algorithm cycles through sort_array,
    comparing adjacent elements and swapping pairs that are out
    of order. It continues to do this until no pairs are swapped.
}
PROCEDURE bubble_sort;
    VAR row,                { Item row is compared to item row + 1 }
        switch,             { Row at which items were switched }
        limit   : Integer;  { Last item - 1 to be compared }

    BEGIN
    limit := max_bars;
    REPEAT
        switch := 0;
        FOR row := 1 TO limit - 1 DO
            { Two adjacent elements are out of order, so swap their
              values and redraw those two bars.
             }
            IF (sort_array[row].len > sort_array[row + 1].len) THEN
                BEGIN
                swap_sort_elements( sort_array[row], sort_array[row + 1] );
                draw_bar_swap( row, row + 1 );
                elapsed_time( row, bubble );
                switch := row;
                END;

        { Sort on next pass only to where the last switch was made. }
        limit := switch;

    UNTIL (switch = 0);
    END; { bubble_sort }


{======================= exchange_sort ==========================
    The exchange_sort compares each element in sort_array,
    starting with the first element, with every following
    element.  If any of the following elements is smaller than
    the current element, it is exchanged with the current
    element and the process is repeated for the next element in
    sort_array.
}
PROCEDURE exchange_sort;
    VAR row,                  { Row being compared }
        least,                { Smallest row encountered }
        j      : Integer;
    BEGIN
    FOR row := 1 TO max_bars - 1 DO
        BEGIN
        least := row;
        FOR j := row + 1 TO max_bars DO
            BEGIN
            IF (sort_array[j].len < sort_array[least].len) THEN
                BEGIN
                least := j;
                elapsed_time( j, exchange );
                END;
            END;
        IF (least > row) THEN
            { This row is shorter than the current row, so swap those
              two array elements.
            }
            BEGIN
            swap_sort_elements( sort_array[row], sort_array[least] );
            draw_bar_swap( row, least );
            elapsed_time( row, exchange );
            END;
        END;
    END;  { exchange_sort }


{======================= heap_sort ==============================
    The heap_sort procedure works by calling two private
    procedures, percolate_up and percolate_down.  percolate_up
    turns sort_array into a "heap," which has the properties
    outlined in the diagram below:

                            sort_array(1)
                           /            \
              sort_array(2)                sort_array(3)
               /       \                   /       \
    sort_array(4)    sort_array(5)   sort_array(6)  sort_array(7)
       /  \            /  \           /  \          /  \
     ...  ...        ...  ...       ...  ...      ...  ...

    where each "parent node" is greater than each of its "child
    nodes".

    For example, sort_array(1) is greater than sort_array(2) or
    sort_array(3). sort_array(3) is greater than sort_array(6) or
    sort_array(7), and so forth.

    So, once the first FOR loop in heap_sort is finished, the
    largest element is in sort_array(1).

    The second FOR loop in heap_sort swaps the element in
    sort_array(1) with the element in max_level, rebuilds the
    heap (with percolate_down) for max_level - 1, then swaps the
    element in sort_array(1) with the element in max_level - 1,
    rebuilds the heap for max_level - 2, and continues in this
    way until the array is sorted.
}
PROCEDURE heap_sort;

    {=================== percolate_down =======================
    The percolate_down procedure restores the elements of
    sort_array from 1 to max_level to a "heap" (see the diagram
    with the heap_sort procedure).
    }
    PROCEDURE percolate_down( max_level : Integer );
        VAR
            i,
            child : Integer;  { Child of element being compared }
            done  : Boolean;  { True when finished }
        BEGIN
        i := 1;
        done := False;

        { Move the value in sort_array(1) down the heap until it
          has reached its proper node (that is, until it is less
          than its parent node or until it has reached max_level,
          the bottom of the current heap):
        }
        WHILE (NOT done) DO
            BEGIN
            { Get the child node. }
            child := 2 * i;
            IF (child > max_level) THEN
                done := True  { Reached the bottom of the heap, so exit }
            ELSE
                BEGIN
                { If there are two children, find the bigger one. }
                IF (child + 1 <= max_level) THEN
                    IF (sort_array[child + 1].len >
                        sort_array[child    ].len) THEN
                        child := child + 1;
                { Move the value down if it is still smaller than
                  either one of its children.
                }
                IF (sort_array[i].len < sort_array[child].len) THEN
                    BEGIN
                    swap_sort_elements( sort_array[i], sort_array[child] );
                    draw_bar_swap( i, child );
                    elapsed_time( i, heap );
                    i := child;
                    END
                ELSE
                    { Sort_array has been restored to a heap from
                      1 to max_level, so exit.
                    }
                    done := True;
                END;
            END;
        END;  { percolate_down }

    {======================= percolate_up =====================
    The percolate_up procedure converts the elements from 1 to
    max_level in sort_array into a "heap" (see the diagram with
    the heap_sort procedure).
    }
    PROCEDURE percolate_up( max_level : Integer );
        VAR
            i,
            parent : Integer;  { Parent of element compared }
        BEGIN
        i := max_level;

        { Move the value in sort_array(max_level) up the heap until
          it has reached its proper node (that is, until it is
          greater than either of its child nodes, or until it has
          reached 1, the top of the heap).
        }
        WHILE (i <> 1) DO
            BEGIN
            parent := i DIV 2;  { Get the subscript for the parent }
            IF (sort_array[i].len > sort_array[parent].len) THEN
                { The current node is still bigger than its parent
                  so swap these two elements.
                }
                BEGIN
                swap_sort_elements( sort_array[parent], sort_array[i] );
                draw_bar_swap( parent, i );
                elapsed_time( parent, heap );
                i := parent;
                END
            ELSE
                { Otherwise, the element has reached its proper
                  place in the heap, so exit this procedure.
                }
                i := 1;
            END;
        END; { percolate_up }

    { ====================================================
      Declarations and code for heap_sort
    }
    VAR
        i : Integer;
    BEGIN
    FOR i := 2 TO max_bars DO percolate_up( i );
    FOR i := max_bars DOWNTO 2 DO
        BEGIN
        swap_sort_elements( sort_array[1], sort_array[i] );
        draw_bar_swap( 1, i );
        elapsed_time( 1, heap );
        percolate_down( i - 1 );
        END;
    END;  { heap_sort }

{======================= insertion_sort ========================
    The insertion_sort procedure compares the length of each
    successive element in sort_array with the lengths of all the
    preceding elements.  When the procedure finds the
    appropriate place for the new element, it inserts the
    element in its new place, and moves all the other elements
    down one place.
}
PROCEDURE insertion_sort;
    VAR
        j,
        row,                    { Row being inserted }
        temp_length : Integer;  { Length of current row }
        temp : sort_elements;   { Current row record }
    BEGIN
    FOR row := 2 TO max_bars DO
        BEGIN
        temp := sort_array[row];
        temp_length := temp.len;
        j := row;
        WHILE ((j >= 2) AND (sort_array[j - 1].len > temp_length)) DO
            BEGIN
            sort_array[j] := sort_array[j - 1];
            print_one_bar( j );              { Print the new bar. }
            elapsed_time( j, insertion );  { Print the elapsed time. }
            Dec( j );
            END;

        { Insert the original value of sort_array(row) in sort_array(j). }
        sort_array[j] := temp;
        print_one_bar( j );                  { Print the new bar. }
        elapsed_time( j, insertion );      { Print the elapsed time. }
        END;
    END;  { insertion_sort }

{ ======================= quick_sort ============================
    Provides a simple procedural interface to the recursive
    qsort.
}
PROCEDURE quick_sort;

    { ======================= qsort =============================
     Picks a random "pivot" element in sort_array, then moves
     every element that is bigger to one side of the pivot, and
     every element that is smaller to the other side.  qsort is
     then called recursively with the two subdivisions created
     by the pivot.  Once the number of elements in a subdivision
     reaches two, the recursive calls end and the array is sorted.
    }
    PROCEDURE qsort( low, high : Integer );
        VAR
            i, j, pivot : Integer;
        BEGIN
        IF (low < high) THEN
            BEGIN
            { Only two elements in this subdivision, so
              order them, then end recursion.
            }
            IF (high - low = 1) THEN
                BEGIN
                IF (sort_array[low].len > sort_array[high].len) THEN
                    BEGIN
                    swap_sort_elements( sort_array[low], sort_array[high] );
                    draw_bar_swap( low, high );
                    elapsed_time( low, quick );
                    END;
                END
            ELSE
                BEGIN
                pivot := sort_array[high].len;
                i := low;
                j := high;
                WHILE (i < j)   DO
                    BEGIN
                    { Move in from both sides towards the pivot element. }
                    WHILE ((i < j) AND (sort_array[i].len <= pivot)) DO
                        Inc( i );
                    WHILE ((j > i) AND (sort_array[j].len >= pivot)) DO
                        Dec( j );
                    { If we haven't reached the pivot element,
                      two elements on either side must be out of order,
                      so swap.
                    }
                    IF (i < j) THEN
                        BEGIN
                        swap_sort_elements( sort_array[i], sort_array[j] );
                        draw_bar_swap( i, j );
                        elapsed_time( i, quick );
                        END;
                    END;

                { Move the pivot back to its proper place. }
                swap_sort_elements( sort_array[i], sort_array[high] );
                draw_bar_swap( i, high );
                elapsed_time( i, quick );

                { Recursively call qsort, sorting smaller partition
                  first to conserve stack space.
                }
                IF ((i - low) < (high - i)) THEN
                    BEGIN
                    qsort( low, i - 1 );
                    qsort( i + 1, high );
                    END
                ELSE
                    BEGIN
                    qsort( i + 1, high );
                    qsort( low, i - 1 );
                    END;
                END;
            END;
        END;  { qsort }

    { =========================================================
      code for quick_sort
    }
    BEGIN
    qsort( 1, max_bars );
    END;


{======================= Shell_sort ==========================
    The Shell_sort procedure is similar to the bubble_sort
    procedure.  However, Shell_sort begins by comparing
    elements that are far apart (separated by the value of the
    offset variable, which is initially half the distance
    between the first and last element), then comparing
    elements that are closer together.  When the offset is one,
    the last iteration of this procedure is merely a bubble sort.
}
PROCEDURE Shell_sort;
    VAR
        offset,            { Comparison offset }
        switch,            { Row where last switch occurred }
        limit,             { Number of elements to compare each pass }
        row     : Integer; { Current row }
    BEGIN
    { Set offset to half the number of element in sort_array. }
    offset := max_bars DIV 2;

    WHILE (offset > 0) DO
        BEGIN
        { Loop until offset gets to zero. }
        limit := max_bars - offset;
        REPEAT
            switch := 0; { No switches at this offset }
            { Compare elements and switch ones out of order. }
            FOR row := 1 TO limit DO
                IF (sort_array[row].len >
                    sort_array[row + offset].len) THEN
                    BEGIN
                    swap_sort_elements( sort_array[row],
                                        sort_array[row + offset] );
                    draw_bar_swap( row, row + offset );
                    elapsed_time( row, shell );
                    switch := row;
                    END;

            { Sort on next pass only to where last switch was made. }
            limit := switch - offset;
        UNTIL (switch = 0);
        { No switches at last offset, so try one half as big. }
        offset := offset DIV 2;
        END;
    END;  { Shell_sort }
{$F-} { Disable FAR calls. }

{======================= screen_setup =======================
    Sets the display to largest number of text rows available
    and sets the number of colors.
}
PROCEDURE screen_setup;
    BEGIN
    IF LastMode = Mono THEN
        BEGIN
        max_colors := 1;
        TextMode( Mono );
        screen_back := Black;
        menu_back   := Black;
        menu_frame  := LightGray;
        menu_text   := LightGray;
        menu_status     := LightGray;
        menu_highlight  := White;
        END
    ELSE
        BEGIN
        max_colors := 15;
        TextMode( CO80 + Font8x8 );
        END;
    max_bars := Hi( WindMax );  { Get number of text rows. }
    IF max_bars > maxmax_bars THEN
        max_bars := maxmax_bars;
    END;  { screen_setup }

{======================= draw_menu =============================
    Calls the draw_frame procedure to draw a frame around the
    sort menu, then prints the choices from the menu array.
}
PROCEDURE draw_menu;

    {======================= draw_frame ========================
    Draws a rectangular frame using the high-order ASCII
    characters #201, #187, #200, #188, #186, and #205.
    The parameters TopSide, BottomSide, LeftSide, and RightSide
    are the row and column arguments for the upper-left and
    lower-right corners of the frame.
    }
    PROCEDURE draw_frame( top, left, menu_width, height : Integer );
        CONST
            uleft      = #201;  { Upper left character }
            uright     = #187;  { Upper right character }
            ureft      = #200;  { Lower left character }
            lright     = #188;  { Lower right character }
            vertical   = #186;  { Vertical line character }
            horizontal = #205;  { Horizontal line character }
        VAR
            line : CSTRING[80]; { Horizontal slice of box }
            i    : Integer;
        BEGIN

        FillChar( line[2], menu_width - 2, horizontal );
        line[menu_width + 1] := null_char;

        line[1] := uleft;
        line[menu_width] := uright;
        GotoXY( left, top );
        Write( line );

        line[1] := ureft;
        line[menu_width] := lright;
        GotoXY( left, top + height );
        Write( line );

        FillChar( line[2], menu_width - 2, space );
        line[1] := vertical;
        line[menu_width] := vertical;

        FOR i := top + 1 TO top + height - 1 DO
            BEGIN
            GotoXY( left, i );
            Write( line );
            END;
        END;  { draw_frame }

    { ==========================================================
      Declarations and code for draw_menu
    }
    CONST
        on_off_str : ARRAY[Boolean] OF STRING[3] = ('OFF', 'ON');
    VAR
        i : Integer;
    BEGIN
    TextBackground( menu_back );
    TextColor( menu_frame );
    draw_frame( 1, menu_left - 3, menu_width + 3, 20 );

    TextColor( menu_text );
    FOR i := 1 TO menu_height DO
        BEGIN
        GotoXY( menu_left, menu_top + i );
        Write( menu[i] );

        IF (i IN [3..10, 13, 14]) THEN
            BEGIN
            TextColor( menu_highlight );
            GotoXY( menu_left, menu_top + i );
            Write( menu[i][1] );
            TextColor( menu_text );
            END;

        END; { FOR }

    TextColor( menu_status );
    { Print the current sound_on setting. }
    GotoXY( menu_left + 14, 11 );
    Write( on_off_str[sound_on] );

    { Display pause setting. }
    GotoXY( menu_left + 13, 13 );
    Write( (pause DIV 20):3 );

    { Erase the speed option if the length of the Pause is at a limit. }
    IF (pause = 900) THEN
        BEGIN
        GotoXY( menu_left, 14 );
        Write( '':12 );
        END
    ELSE IF (pause = 0) THEN
        BEGIN
        GotoXY( menu_left, 15 );
        Write( '':12 );
        END;
    TextBackground( screen_back );
    END;  { draw_menu }

{======================= initialize ============================
    Initializes the unsorted_array.
}
PROCEDURE initialize;
    VAR
        i,
        index,
        max_index,             { Max index for initialization }
        bar_length : Integer;  { Length of initialized bar }
        temp : ARRAY[1..maxmax_bars] OF Integer;  { Bar array }
    BEGIN

    Randomize;
    FOR i := 1 TO max_bars DO temp[i] := i;
    max_index := max_bars;
    FOR i := 1 TO max_bars DO
        BEGIN
        { Find a random element in temp between 1 and max_index,
          then assign the value in that element to bar_length.
        }
        index := rand_int( 1, max_index );
        bar_length := temp[index];
        { Overwrite the value in temp[index] with the value in
          temp[max_index] so the value in temp[index] is
          chosen only once.
        }
        temp[index] := temp[max_index];
        { Decrease the value of max_index so that temp[max_index]
          can't be chosen on the next pass through the loop.
        }
        Dec( max_index );

        unsorted_array[i].len := bar_length;
        IF (max_colors = 1) THEN
            unsorted_array[i].color := LightGray
        ELSE
            unsorted_array[i].color := (bar_length MOD max_colors) + 1;
        END;

    END;  { initialize }


{======================= reinitialize ==========================
    Restores the array sort_array to its original unsorted
    state, then draws the unsorted color bars.
}
PROCEDURE reinitialize;
    VAR
        row      : Integer;  { Bar array indexing }
    BEGIN

    FOR row := 1 TO max_bars DO
        BEGIN
        sort_array[row] := unsorted_array[row];
        print_one_bar( row );
        END;

    { Read start time in clock ticks from predefined memory array. }
    start_time := MemL[$40:$6C];
    END; { Reinitialize }


{======================= sort_menu ===========================
    Prompts the user to make one of the following choices:
        - One of the sorting algorithms
        - Toggle sound on or off
        - Increase or decrease speed
        - End the program
}
PROCEDURE sort_menu;

    {======================= do_sort ========================
     Initializes the array and execute the selected sort,
     printing its elapsed time.
    }
    PROCEDURE do_sort( which_sort : sort_range );

        {======================= toggle_cursor ==============
         Changes the cursor's visibility.
         Returns True if the cursor is visible after being
         toggled.
         Assumes video page 0.
        }
        FUNCTION toggle_cursor : Boolean;
            VAR
                r : Registers;
            BEGIN
            r.AH := 3;  { Read cursor function }
            r.BH := 0;  { Assume page 0 }
            Intr( 16, r );
            r.AH := 1;
            r.CH := r.CH XOR $20; { Toggle visible bit }
            toggle_cursor := ( r.CH AND $20 ) = 0;
            Intr( 16, r );
            END; { toggle_cursor }

        { ==================================================
          Declarations and code for do_sort
        }
        VAR
            b : Boolean;  {  Dummy for toggle_cursor }
        BEGIN
        reinitialize;   { Restore the array to the unsorted state. }
        IF toggle_cursor THEN           { Make sure cursor is off. }
            b := toggle_cursor ;
        sort[which_sort];               { Sort. }
        elapsed_time( 0, which_sort );  { Print final time. }
        b := toggle_cursor;             { Turn cursor back on. }
        END; { do_sort }

    { =======================================================
           Declarations and code for sort_menu
    }
    VAR
        ch   : Char;     { Character read from keyboard }
        done : Boolean;  { True when ESC pressed }
    BEGIN
    done := False;
    WHILE NOT done DO
        BEGIN
        GotoXY( menu_left + Length( menu[menu_height] ),
                menu_top + menu_height );
        ch := UpCase( ReadKey );
        CASE ch OF
            'I' : do_sort( insertion );
            'B' : do_sort( bubble );
            'H' : do_sort( heap );
            'E' : do_sort( exchange );
            'S' : do_sort( shell );
            'Q' : do_sort( quick );
            '>',
            '.' :  { Decrease pause. }
                IF (pause > 0) THEN
                    BEGIN
                    pause := pause - 20;
                    draw_menu;  { Erase old timings--no longer valid. }
                    END;
            '<',
            ',' :  { Increase pause }
                IF (pause < 900) THEN
                    BEGIN
                    pause := pause + 20;
                    draw_menu;  { Erase old timings--no longer valid. }
                    END;
            'T' :   { Toggle sound. }
                BEGIN
                sound_on := NOT sound_on;
                draw_menu;
                END;
            esc : done := True;
            #0  : ch := ReadKey;
            ELSE  { Ignore any other input. }
            END;
        END;
    END;  { sort_menu }


{=================== Main program ============================}
BEGIN

    { Initialize array of sort procedures. }
    sort[insertion] := insertion_sort;
    sort[bubble]    := bubble_sort;
    sort[heap]      := heap_sort;
    sort[exchange]  := exchange_sort;
    sort[shell]     := Shell_sort;
    sort[quick]     := quick_sort;

    start_mode      := LastMode;  { Save starting video mode. }
    screen_setup; { Set up display, get max_bars, max_colors. }

    scale_frequency := 5000 DIV max_bars;  { Scaling factor for pitch }
    sound_on        := True;               { Sound is on by default }
    pause           := 60;                 { Default for pause }

    initialize;   { Initialize data values }
    { Assign values in unsorted_array to sort_array
      and draw unsorted bars on the screen.
    }
    reinitialize;
    draw_menu;
    sort_menu;    { Activate sort menu }

    TextMode( start_mode );  { Restore startup video mode }

END.
