program hanoi( output ); { towers of hanoi }

const
   n  = 5;   { number of disks }
   n2 = 10;  { 2 * n }
   n3 = 15;  { n2 + 5 }

var
   p1, p2, p3 : integer;  { position of top disk on each pole }
   disks : packed array [ 1..n, 1..n2 ] of char; { disk formations }
   pole1, pole2, pole3 : array [ 1..n ] of integer; { holds which disks are }
   blank : array[ 1..n3 ] of char;                  { on which poles }

procedure initialize( disk : integer );
   var
      i, j : integer; { used for  'for' loops, and accessing array 'disks' }
   begin
      createblank;
      for i := disk downto 1 do
          begin
             pole1[ i ] := i;                   { initialize arrays }
             pole2[ i ] := 0; pole3[ i ] := 0;  { pole1, pole2, pole3 }
             for j := 1 to disk * 2 do
                 disks[ i, j ] := ' ';          { clear array 'disks' }
          end;
      for i := disk downto 1 do
          for j := i to  disk * 2 - i do
              disks[ i , j ] := '*'             { initialize disk formations }
   end;

procedure putadisk( disk : integer ); { puts a specified disk on pole1 or 'a' }
   begin
      p1 := p1 + 1;
      pole1[ p1 ] := disk;
   end;

procedure getadisk;               { gets top disk from pole1 or 'a' }
   begin
      pole1[ p1 ] := 0;
      p1 := p1 - 1;
   end;

procedure putbdisk(disk:integer); { puts a specified disk on pole2 or 'b' }
   begin
      p2 := p2 + 1;
      pole2[ p2 ] := disk;
   end;

procedure getbdisk;               { gets top disk from pole2 or 'b' }
   begin
      pole2[ p2 ] := 0;
      p2 := p2 - 1;
   end;

procedure putcdisk(disk:integer); { puts a specified disk on pole3 or 'c' }
    begin
       p3 := p3 + 1;
       pole3[ p3 ] := disk;
    end;

procedure getcdisk;               { gets top disk from pole3 or 'c' }
   begin
      pole3[ p3 ] := 0;
      p3 := p3 - 1;
   end;

procedure movedisk(disk:integer; x,y:char); { moves disks from one pole
                                              to another }
   begin
      if x = 'a' then
         begin
            if y = 'b' then
                putbdisk( pole1[ p1 ] )
            else
                putcdisk( pole1[ p1 ] );
            getadisk;
         end
      else if x = 'b' then
         begin
            if y = 'a' then
                putadisk( pole2[ p2 ] )
            else
                putcdisk( pole2[ p2 ] );
            getbdisk;
         end
      else
         begin
            if y = 'a' then
                putadisk( pole3[ p3 ] )
            else
                putbdisk( pole3[ p3 ] );
            getcdisk;
         end;
      drawdisks;
   end;

procedure drawdisks;
   var
      i : integer;
   begin
      poscursor( 7, 1 );
      for i:=n downto 1 do
          begin
             if pole1[ i ] <> 0 then
                write( disks[ pole1[ i ] ], '     ' )
             else
                write( blank );
             if pole2[ i ] <> 0 then
                write( disks[ pole2[ i ] ], '     ' )
             else
                write( blank );
             if pole3[ i ] <> 0 then
                write( disks[ pole3[ i ] ], '     ' )
             else
                write( blank );
             writeln;
          end;
   end;


procedure createblank;
   var
      j : integer;
   begin
      for j := 1 to n3 do
          blank[ j ] := ' ';
      blank[ n ] := '|';
   end;

procedure hanoi( n : integer; a, c, b : char );
   begin
      if n > 0 then
         begin
            hanoi( n - 1, a, b, c );
            movedisk( n, a, c );
            hanoi( n - 1, b, c, a)
         end
   end;

begin  {  main  }
   page;
   poscursor( 4, 1 );
   writeln( 'Executing for ', n : 1, ' disks' );
   initialize( n );
   p1 := n;
   p2 := 0;
   p3 := 0;
   drawdisks;
   hanoi( n, 'a', 'c', 'b' );
   poscursor( 14, 1 );
   writeln( 'How About That ?...' );
end.
