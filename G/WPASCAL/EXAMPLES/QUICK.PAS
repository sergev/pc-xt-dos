program quicksort( output );

  const
    n = 32;

  var
    vector : array [ 1..n ] of integer;
    z : integer;

  procedure swap( x, y : integer );
    var
      temp : integer;
    begin {swap}
      temp := vector[x];
      vector[x] := vector[y];
      vector[y] := temp
    end; { swap }

  procedure quicksort( l, r : integer );
    var
      i, j : integer;
    procedure partition_array(l, r : integer);
      var
        pivot_value : integer;
      begin { partition_array }
        i := l;
        j := r;
        pivot_value := vector[ (l+r) div 2 ];
        repeat
          while vector[i] < pivot_value do
            i := i + 1;
          while vector[j] > pivot_value do
            j := j - 1;
          if i <= j then
            begin
              swap( i, j );
              i := i + 1;
              j := j - 1;
            end
        until i > j
      end; { partition_array }

    begin { quicksort }
      if l + 1 = r then
        begin
          if vector[l] > vector[r] then
            swap( l, r );
        end
      else if l + 1 < r then
        begin
          partition_array( l, r );
          quicksort( l, j );
          quicksort( i, r );
        end
    end;

  begin
    writeln;
    writeln( 'Here are the unsorted numbers' );
    writeln;
    for z:= 1 to n do
      begin
        vector[ z ] := random;
        write( vector[ z ]:5 );
      end;
    writeln;
    quicksort( 1, n );
    writeln;
    writeln( 'Here are the sorted numbers' );
    writeln;
    for z := 1 to n do
        write( vector[ z ]:5 );
  end.
