UNIT bigheap;
{
    BIGHEAP.PAS

    New versions of GetMem and FreeMem to allow allocation of
    memory blocks larger than 65520 bytes.  

    These routines may be used to manage heap memory blocks
    larger than the maximum allowed by the predefined GetMem 
    and FreeMem.  They are especially useful for graphics 
    image buffers which can easily exceed 65520 bytes.      
}

{===============================================================}
                INTERFACE
{===============================================================}


VAR HeapError : Pointer;  { heap error procedure }

{ new long block versions of the heap management procedures }
PROCEDURE GetMem ( VAR p : Pointer; request : LongInt );
PROCEDURE FreeMem( VAR p : Pointer; size    : LongInt );

{===============================================================}
                IMPLEMENTATION
{===============================================================}


TYPE
    { variant record used to access the segment and offset
      parts of the pointer 
    }
    pointer_rec = RECORD CASE Integer OF
        1 : ( p : Pointer );
        2 : ( w : RECORD
                offset,
                segment : Word
                END
            );
        END;

    { elements of the free list }
    free_rec = RECORD
        origin,      { points to the start of the free block }
        termination  { points to the end of the free block }
            : Pointer;
        END;

    free_list  = ARRAY[0..8189] OF free_rec;
    pfree_list = ^free_list;

    { type of new heap error functions }
    heap_error_functions    = FUNCTION( s : LongInt ) : Integer;

    { type of old heap error functions }
    old_heap_error_function = FUNCTION( s : Word    ) : Integer;

CONST
    largest_block = 65520;

{=============== free_count ====================================
    return the number of blocks in the free list
}
FUNCTION free_count : Integer;
    BEGIN
    free_count := (8192 - Ofs(FreePtr^) DIV 8) MOD 8192;
    END;


{=============== denormal ======================================
    return the denormalized form of a pointer
}
FUNCTION denormal( p : Pointer ) : LongInt;
    VAR cast : pointer_rec;  { pointer conversion record }
    BEGIN
    cast.p := p;
    denormal := (LongInt(cast.w.segment) SHL 4) + cast.w.offset;
    END;


{=============== normalize =====================================
    return the normalized form of a pointer
}
FUNCTION normalize( p : Pointer ) : Pointer;
    VAR cast : pointer_rec;  { pointer conversion record }
    BEGIN
    cast.p := p;
    cast.w.segment := cast.w.segment SHL 12 + (cast.w.offset SHR 4);
    cast.w.offset := cast.w.offset AND $f;
    normalize := cast.p;
    END;


{=============== inc_normal ====================================
    increment a normalized pointer
}
PROCEDURE inc_normal( VAR p : Pointer; increment : LongInt);
    BEGIN
    p := normalize( Pointer(denormal(p) + increment) );
    END;


{=============== diff_normals =================================
    return the difference between two normalized pointers
}
FUNCTION diff_normals( p1, p2 : Pointer ) : LongInt;
    BEGIN
    diff_normals := denormal(p1) - denormal(p2);
    END;


{=============== free_block_size ===============================
    return the size of a free block
}
FUNCTION free_block_size( block : free_rec ) : LongInt;
    BEGIN
    free_block_size := diff_normals( block.termination, block.origin);
    END;


{=============== remove_block ==================================
    remove a block from the free list
}
PROCEDURE remove_block( VAR block : free_rec );
    TYPE
        union = RECORD CASE Integer OF
            1 : ( p   : Pointer );
            2 : ( fl_ptr : pfree_list );
            3 : ( w   : RECORD offset, segment : Word END );
            END;
    VAR cast : union;  { pointer conversion record }
    BEGIN
    cast.p  := FreePtr;
    block   := cast.fl_ptr^[0];
    FreePtr := Ptr( cast.w.segment, cast.w.offset + SizeOf(free_rec) );
    END;


{=============== pool_size =====================================
    return the size of the free pool    
}
FUNCTION pool_size : LongInt;
    VAR cast : pointer_rec;  { pointer conversion record }
    BEGIN
    cast.p := FreePtr;
    IF (cast.w.offset = 0) THEN Inc( cast.w.segment, $1000 );
    pool_size := diff_normals( cast.p, HeapPtr );
    END;


{=============== FAR heap functions ============================
{$F+}

{=============== heap_function =================================
    new default HeapErr function
    0 means to terminate with runtime error 203
}
FUNCTION heap_function( size : LongInt ) : Integer;
    BEGIN
    heap_function := 0;
    END;

{=============== small_heap_function ===========================
    replacement for old HeapErr function 
    calls the new heap error function (or user replacement)
    NOTE: uses a typecast to call the function in the HeapError
          global variable
}
FUNCTION small_heap_function( size : Word ) : Integer;
    BEGIN
    small_heap_function := heap_error_functions(HeapError)(size);
    END;

{$F-}
{=============== end of FAR heap functions =====================



{=============== GetMem ========================================
    long version of GetMem
}
PROCEDURE GetMem( VAR p : Pointer; request : LongInt);
    LABEL
        TryAgain, Carve;
    VAR
        fl_ptr       : pfree_list;
        last_block, count : Integer;
        action    : Integer;
    BEGIN
    { Use normal heap routines if request is small }
    IF (request <= largest_block) THEN
        BEGIN
        System.GetMem( p, Word(request) );
        Exit;
        END;

    { Check for an entry on the free list that is large enough for block }
TryAgain:
    last_block := free_count;
    IF (last_block = 0) THEN  { no free blocks } 
        GOTO Carve;

    fl_ptr := FreePtr;
    FOR count := 0 TO last_block - 1 DO
        IF (free_block_size( fl_ptr^[count] ) >= request) THEN
            BEGIN

            { found a block, split it into two blocks }
            p := fl_ptr^[count].origin;

            { adjust start pointer }
            inc_normal( fl_ptr^[count].origin, request );

            { the block drops out of the free list if nothing left }
            IF (free_block_size( fl_ptr^[count] ) = 0) THEN
                remove_block( fl_ptr^[count] );
            { done }
            Exit;
            END;

    { If here, there are no blocks on the free list large enough.
      We have to carve it out of the pool.
    }
Carve:
    { Is there enough memory in the pool? }
    IF (pool_size < request + FreeMin) THEN
        BEGIN
        { Not enough memory, call HeapError function
          Note the use of a typecast to make the indirect call 
          through the global HeapError Pointer variable.    
        }
        action := heap_error_functions(HeapError)( request );
        CASE action OF
            1 :    { return a NIL pointer to the program }               
                BEGIN
                p := NIL;
                Exit;
                END;
            2 :    { retry allocation }                 
                GOTO TryAgain;  
            ELSE   { generate heap overflow runtime error }
                RunError( 203 );  
            END;
        END;

    { Yes, there is enough memory }
    p := HeapPtr;
    inc_normal( HeapPtr, request );
    END;


{=============== FreeMem ======================================
    long version of FreeMem
}
PROCEDURE FreeMem( VAR p : Pointer; size : LongInt);
    LABEL
        Again;

    BEGIN
Again:
    IF (size <= largest_block) THEN
        BEGIN
        System.FreeMem( p, Word(size) );
        Exit;
        END;
    System.FreeMem( p, largest_block );
    inc_normal( p, largest_block );
    Dec( size, largest_block );
    GOTO Again;
    END;



{=============== unit INITIALIZATION ==========================}
BEGIN
HeapError := @heap_function;
System.HeapError := @small_heap_function;

{ Adjust FreeMin to allow room for FreeList when deallocating
  large blocks.  Allow for 3 free list entries }
Inc( System.FreeMin, 3 * SizeOf( free_rec ) );
END.
