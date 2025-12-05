PROGRAM CRLF;
{
    CRLF.PAS
    Normalizes all line endings of a text file to CRLF.

    USAGE: CRLF <file> [<file>]...
    Output file will have the same root name and extension .OK

    Full diagnostic and progress information is displayed.
}

{$V-,R-,S+,A+,B-,N-}
{$M 1024,0,0 }{ no heap needed }

USES
    Dos;

CONST
    max_buf  = 4 * 1024;  { buffer size }
    out_ext  = '.OK';     { output file extension }

TYPE
    { status returned by open_files }
    file_status = ( ok, no_input, no_output );

VAR
    { I/O files }
    file_in,
    file_out : FILE;

    { I/O buffers }
    buffer_in,
    buffer_out : ARRAY[1..max_buf] OF Char;

    { file names & parts }
    Name,
    extension,
    file_in_name,
    file_out_name : STRING;

    { index into command line }
    i : Integer;

{=============== usage ========================================
    tell how to use this program
}
PROCEDURE usage;
    BEGIN
    Writeln;
    Writeln( 'CRLF : makes sure all line endings in a text file' );
    Writeln( '       are carriage-return/linefeed sequences.' );
    Writeln;
    Writeln( 'USAGE: CRLF <file> [<file>]...' );
    Writeln;
    Writeln( '  Output file will have the extension .OK' );
    Writeln( '  If file is clean, the output file is deleted.' );
    Writeln( '  The original file is never changed.' );
    Writeln;
    END;

{=============== open_files ===================================
    opens the input and output files, returning status
}
FUNCTION open_files : file_status;
    BEGIN
    {$I-}
    Assign( file_in, file_in_name);
    Reset( file_in, 1 );
    {$I+}
    IF IOResult <> 0 THEN
        open_files := no_input
    ELSE
        BEGIN
        {$I-}
        Assign( file_out, file_out_name );
        Rewrite( file_out, 1 );
        {$I+}
        IF IOResult = 0 THEN
            open_files := ok
        ELSE
            open_files := no_output;
        END;
    END;  { open_files }

{=============== close_files ==================================
    close the files
}
PROCEDURE close_files;
    BEGIN
    Close(file_in);
    Close(file_out);
    END;

{=============== clean_file ===================================
    clean up the file
    Make sure all carriage-returns and linefeeds are properly
    paired.
    Returns True if file was clean.
}
FUNCTION clean_file : Boolean;
    TYPE
        states =
            ( sCh,  { scanning characters }
              sCR,  { scanning carriage returns }
              sLF   { scanning linefeeds }
            );
    CONST
        CR = #13;   { ASCII carriage-return }
        LF = #10;   { ASCII linefeed }
    VAR
        scan,             { current buffer index }
        cur_out,          { current output buffer index }
        bytes : Word;     { number of bytes transferred }
        state : states;   { current scan state }
        clean,            { set False if anything dirty }
        done  : Boolean;  { finished flag }

    {=========== put ==========================================
      put a character in the output buffer, flush buffer to
      disk if full
    }
    PROCEDURE put( c : Char );
        BEGIN
        buffer_out[cur_out] := c;
        Inc(cur_out);
        IF cur_out > max_buf THEN
            BEGIN
            BlockWrite( file_out, buffer_out, max_buf, bytes );
            cur_out := 1;
            END;
        END;  { put }

    {=========== clean_file body ==============================}
    BEGIN
    BlockRead( file_in, buffer_in, SizeOf(buffer_in), bytes );
    scan    := 1;
    cur_out := 1;
    state   := sCh;
    done    := False;
    clean   := True;  { assume everything is ok }
    REPEAT
        CASE state OF
            sCh :
                CASE buffer_in[scan] OF
                    LF :        { LF with no preceding CR }
                        BEGIN
                        clean := False;
                        put(CR);
                        put(LF);
                        state := sLF;
                        END;
                    CR :        { CR, expect a LF next }
                        BEGIN
                        put(CR);
                        state := sCR;
                        END;
                    ELSE        { copy other characters through }
                        put(buffer_in[scan]);
                    END;
            sCR :
                CASE buffer_in[scan] OF
                    LF :        { found expected LF }
                        BEGIN
                        put(LF);
                        state := sLF;
                        END;
                    CR :        { CR, missing expected LF }
                        BEGIN
                        clean := False;
                        put(LF);
                        put(CR);
                        END;
                    ELSE        { char, missing expected LF }
                        clean := False;
                        put(LF);
                        put(buffer_in[scan]);
                        state := sCh;
                    END;
            sLF :
                CASE buffer_in[scan] OF
                    LF :       { LF with no preceding CR }
                        BEGIN
                        clean := False;
                        put(CR);
                        put(LF);
                        END;
                    CR :       { CR, expect a LF next }
                        BEGIN
                        put(CR);
                        state := sCR;
                        END;
                    ELSE
                        put(buffer_in[scan]);
                        state := sCh;
                    END;
            END;

        IF (bytes < SizeOf(buffer_in)) AND (scan = bytes) THEN
            done := True
        ELSE
            BEGIN
            { advance to next character }
            Inc(scan);
            IF scan > max_buf THEN
                BEGIN
                BlockRead( file_in, buffer_in, SizeOf(buffer_in), bytes );
                scan := 1;
                END;
            END;

    UNTIL done;

    IF state = sCR THEN  { file ended in bare CR }
        BEGIN
        put(LF);
        clean := False;
        END;

    { flush remaining output, if any }
    IF cur_out > 1 THEN
        BlockWrite( file_out, buffer_out, cur_out - 1, bytes );

    clean_file := clean;
    END;  { clean_file }

{=============== main program body ============================}

BEGIN

IF ParamCount = 0 THEN
    usage  { display usage if no parameters }
ELSE
    BEGIN
    FOR i := 1 TO ParamCount DO
        BEGIN
        FSplit( ParamStr(1), file_in_name, Name, extension );
        file_out_name := file_in_name + Name + out_ext;
        file_in_name  := file_in_name + Name + extension;
        IF (extension = out_ext) THEN
            BEGIN
            Writeln( 'Error: Input file cannot have extension .OK' );
            Writeln( '       Rename the file and try again.' );
            Halt(1);
            END;

        CASE open_files OF
            ok :
                BEGIN
                Writeln( 'Examining file : ', file_in_name );
                IF clean_file THEN
                    BEGIN
                    close_files;
                    Erase( file_out );
                    Writeln( 'All line endings ok--no output' );
                    Writeln;
                    END
                ELSE
                    BEGIN
                    close_files;
                    Writeln( 'Bad line endings found' );
                    Writeln( 'Corrected file is : ', file_out_name );
                    Writeln;
                    END;
                END;
            no_input :
                BEGIN
                Writeln( 'Error: Could not open file : ', file_in_name );
                Writeln( 'Continuing...' );
                Writeln;
                END;
            no_output :
                BEGIN
                Writeln( 'Error: Could not open output file' );
                Halt(1);
                END;
            END;
        END;
    END;
END.
