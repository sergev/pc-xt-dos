C
C This program evaluates and prints the trigonometric sine
C of ten numbers using two methods:
C
C        1. A function, MYSIN, which uses a Taylor
C           series approximation.
C
C        2. The intrinsic function SIN from the FORTRAN/2
C           library.
C
      PROGRAM   DEMO
      REAL MYSIN,X(10,3)
      INTEGER NUMBER(10)
      PRINT '(10X,''X'',10X,''MYSIN(X)'',11X,''SIN(X)'',11X,
     *       ''TERMS'',/)'
      DO 10 I = 1, 10
      VAL = I * 0.1
      X(I,1) = VAL
      X(I,2) = MYSIN(VAL,N)
      X(I,3) = SIN(VAL)
10    NUMBER(I) = N
      PRINT 20, (( X(J,I),I=1,3),NUMBER(J),J=1,10)
      STOP
20    FORMAT(7X, F5.1, 8X, F10.8, 9X, F10.8, 8X, I3)
      END
C
C This function computes the sine of the first argument P1 using
C a Taylor series.  It also returns in P2 the number of Taylor
C series terms required.
C
      REAL FUNCTION MYSIN(P1,P2)
      REAL P1
      DOUBLE PRECISION FACTRL
      INTEGER P2
C
      MYSIN = P1
      DO 10 P2 = 1,100
      DELTA = (-1)**P2 * P1**(2*P2+1) / FACTRL(2*P2+1)
      IF (ABS(DELTA) .LT. 1E-7) RETURN
10    MYSIN = MYSIN + DELTA
      RETURN
      END
C
C This function evaluates the factorial of the integer argument.
C
      DOUBLE PRECISION FUNCTION FACTRL(I)
      FACTRL = 1
      DO 10 K=2,I
10    FACTRL = FACTRL*K
      RETURN
      END
