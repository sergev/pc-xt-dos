            /* Program 10 */
domains
       n, f = integer
predicates
       factorial(n,f)
clauses
       factorial(1,1).
       factorial(N,Res) if
              N > 0 and
              N1 = N-1 and
              factorial(N1,FacN1) and
              Res = N*FacN1.
