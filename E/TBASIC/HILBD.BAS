'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
'³				HILBD.BAS				     ³
'³			       Version 1.0				     ³
'³									     ³
'³	This program demonstrates the increased speed and precision	     ³
'³	of the TURBO-BASIC compiler:					     ³
'³									     ³
'³    --------------------------------------------------		     ³
'³    From: BASIC Programs for Scientists and Engineers		     	     ³
'³									     ³
'³    Alan R. Miller, Sybex						     ³
'³    n x n inverse hilbert matrix					     ³
'³    solution is 1 1 1 1 1						     ³
'³    double-precision version						     ³
'³    --------------------------------------------------                     ³
'³									     ³
'³	The program performs simultaneous solution by Gauss-Jordan           ³
'³	elimination.							     ³
'³									     ³
'³	In order to run this program do the following:			     ³
'³	  1. Load Turbo Basic by typing TB at the DOS prompt. 		     ³
'³	  2. Load the file HILBD.BAS from the Load option of the File	     ³
'³	     pulldown menu.						     ³
'³	  3. Select Run from the Main menu				     ³
'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ


DEFINT I-N: DEFDBL A-H: DEFDBL O-Z
DIM Z(12), A(12,12), COEF(12), B(12,12)

CLS
Max = 12
Nrow = 2
A(1,1) = 1.0D0
PRINT "Inversion of Hilbert Matrices"
DO
  CALL GetData(A(), Z(), Nrow)
  FOR I = 1 TO Nrow
    FOR J = 1 TO Nrow
      B(I,J) = A(I,J)
    NEXT J
  NEXT I
  CALL Gaussj(B(), Z(), Coef(), Nrow, Ierr)
  IF Ierr THEN END                    ' Singular matrix
  CALL WriteData(A(), Z(), Coef(), Nrow)
  Nrow = Nrow+1
LOOP UNTIL Nrow = Max
END ' done

SUB GetData(A(2), Z(1), Nrow)
  FOR I = 1 TO Nrow-1
    A(Nrow,I) = 1.0D0 / (Nrow+I-1)
    A(I,Nrow) = A(Nrow,I)
  NEXT I
  A(Nrow,Nrow) = 1.0D0 / (2*Nrow -1)
  FOR I = 1 TO Nrow
    Z(I) = 0
    FOR J = 1 TO Nrow
      Z(I) = Z(I) + A(I,J)
    NEXT J
  NEXT I
END SUB                               ' GetDataput

SUB WriteData(A(2), Z(1), Coef(1), N)
  SHARED Determ
  IF N < 6 THEN 'show only smaller sets
    PRINT
    PRINT "         Matrix   Constants"
    FOR I = 1 TO N
      FOR J = 1 TO N
       PRINT USING " #.####^^^^^ "; A(I,J);
      NEXT J
      PRINT USING " = #.####^^^^^"; Z(I)
    NEXT I
  END IF
  PRINT
  PRINT "    Solution for "; N; " equations,";
  PRINT " Determinant =";
  PRINT USING " #.#####^^^^^"; Determ
  FOR I = 1 TO N
    PRINT USING " #.######"; Coef(I);
  NEXT I
  PRINT
END SUB ' WriteData

'
'Gauss-Jordan matrix inversion and solution
'
'  B(N,N) is coeff matrix, becomes inverse
'  Y(N) is original constant vector
'  W(N,Nvec) has constant vectors, becomes solution
'  Determ is determinant
'
SUB Gaussj(B(2), Y(1), Coef(1), N, Ierr)
  SHARED Determ                       ' Determinant available if needed
  DIM W(8,1), Index(8,3)
  False% = 0: True% = NOT False%
  Ierr = False%                       'becomes 1 for singular matrix
  Invers = 1                          'print inverse matrix if zero
  Nvec = 1                            'number of constant vectors
  FOR I = 1 TO N
    W(I,1) = Y(I)
    Index(I,3) = 0
  NEXT I
  Determ = 1.0
  FOR I = 1 TO N
    'search for largest (pivot) element
    Big = 0.0
    FOR J = 1 TO N
    IF (Index(J,3) <> 1) THEN
      FOR K = 1 TO N
        IF (Index(K,3) > 1) THEN Emess
        IF (Index(K,3) <> 1) THEN
          IF (Big < ABS(B(J,K))) THEN
            Irow = J
            Icol = K
            Big = ABS(B(J,K))
          END IF
        END IF
       NEXT K
     END IF
   NEXT J
   Index(Icol,3) = Index(Icol,3) + 1
   Index(I,1) = Irow
   Index(I,2) = Icol
   'interchange rows to put pivot on diagonal
   IF (Irow <> Icol) THEN
     Determ = - Determ
     FOR L = 1 TO N
       SWAP B(Irow,L), B(Icol,L)
     NEXT L
     IF Nvec > 0 THEN
       FOR L = 1 TO Nvec
         SWAP W(Irow,L), W(Icol,L)
       NEXT L
     END IF
   END IF
   'divide pivot row by pivot element
   Pivot = B(Icol, Icol)
   IF Pivot = 0.0 THEN Emess
   Determ = Determ * Pivot
   B(Icol,Icol) = 1.0
   FOR L=1 TO N
     B(Icol,L) = B(Icol,L) / Pivot
   NEXT L
   IF Nvec > 0 THEN
     FOR L = 1 TO Nvec
       W(Icol,L) = W(Icol,L) / Pivot
     NEXT L
   END IF
   'reduce nonpivot rows
   FOR L1 = 1 TO N
    IF (L1 <> Icol) THEN
      T = B(L1, Icol)
      B(L1,Icol) = 0.0
      FOR L = 1 TO N
        B(L1,L) = B(L1,L) - B(Icol,L) * T
      NEXT L
      IF Nvec > 0 THEN
        FOR L = 1 TO Nvec
          W(L1,L) = W(L1,L) - W(Icol,L) * T
        NEXT L
      END IF
    END IF
   NEXT L1
  NEXT I
  'interchange columns
  FOR I = 1 TO N
    L = N - I + 1
    IF (Index(L,1) <> Index(L,2)) THEN
      Irow = Index(L,1)
      Icol = Index(L,2)
      FOR K = 1 TO N
        SWAP B(K,Irow), B(K,Icol)
      NEXT K
    END IF
  NEXT I
  FOR K = 1 TO N
    IF (Index(K,3) <> 1) THEN Emess
  NEXT K
  FOR I = 1 TO N
    Coef(I) = W(I,1)
  NEXT I
  EXIT SUB                            ' normally
Emess:
  Ierr = True%
  PRINT "Matrix singular "
END SUB                               ' Gauss-Jordan
