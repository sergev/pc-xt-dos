PROGRAM big_mem;
{ ========================= big_mem ============================
  Big_mem.pas uses the sample BigHeap unit.  Calls in BigHeap
  allow a program to allocate heap space in block larger than
  65520 bytes.
}  

USES 
    bigheap;

VAR
    generic_ptr : POINTER;
    request     : LongInt;
    total       : LongInt;
    biggest     : LongInt;

BEGIN

    biggest := MaxAvail;
    Writeln( 'Total of ', MemAvail, ' bytes available on heap.' );
    Writeln( 'Largest contiguous block is ', biggest, ' bytes.' );

    REPEAT
        Writeln( 'You can allocate up to ', biggest, ' bytes.' );
        Write( 'How large a block of memory do you want to allocate? ');
        Readln( request );
        IF (request > biggest) OR (request < 0)
            THEN Writeln( 'That amount is not available.' );
    UNTIL (request <= biggest) AND
          (request >= 0);
    Writeln( 'Requesting ', request, ' bytes.' );

    { Request memory. This call will use bigheap.GetMem rather than
      System.GetMem, so that we can allocate more than 65520 bytes at
      a time.
    }
    GetMem( generic_ptr, request );
    Writeln( 'This leaves a total of ', MemAvail, ' bytes, with the ');
    Writeln( 'largest block being ', MaxAvail, ' bytes.');

    Writeln( 'Freeing space...' );
    FreeMem( generic_ptr, request );

END.




