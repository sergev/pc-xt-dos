MODULE Hello;
  IMPORT ProgArgs, Terminal;

  VAR arg : ARRAY [0 .. 127] OF CHAR;
      idx : CARDINAL;

BEGIN
  FOR idx := 0 TO ProgArgs.ArgNumber() - 1 DO
    ProgArgs.GetArg(idx,arg);
    Terminal.WriteString(arg);
    Terminal.WriteLn;
  END;
END Hello.
