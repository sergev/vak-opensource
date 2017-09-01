begin comment These examples were described in [D.E.Knuth, J.N.Merner.
         Algol 60 Confidential, Comm.A.C.M., 4, 6 (1961), pp.268-272].
         The authors show possibilities of recursive procedures and
         call-by-name parameters that based on examples of usage of
         the procedure GPS (General Problem Solver) in some assignment
         statements. One of those statements performing matrix multi-
         plication was offered as a challenge to Algol 60 translator
         developers;

      real procedure GPS(I, N, Z, V); real I, N, Z, V;
      begin for I := 1 step 1 until N do Z := V;
         GPS := 1
      end;

      comment The first example demonstrates usage of the procedure
         GPS to create a matrix A[i,j] = i+j;

first example:
      begin
         real i, j;
         array A[1:2,1:3];
         outstring(1, "First example\n");
         i := GPS(j, 3.0, i, GPS(i, 2.0, A[i,j], i+j));
         outstring(1, "Matrix A:\n");
         outreal(1, A[1,1]); outreal(1, A[1,2]); outreal(1, A[1,3]);
         outstring(1, "\n");
         outreal(1, A[2,1]); outreal(1, A[2,2]); outreal(1, A[2,3]);
         outstring(1, "\n")
      end of first example;

      comment The second example demonstrates usage of the procedure
         GPS to perform matrix multiplication;

second example:
      begin

         procedure testmatr(n) result:(a);
            comment create test matrix (CACM, Algorithm 52);
            value n; integer n; array a;
         begin real c, d; integer i, j;
            c := n * (n + 1) * (2 * n - 5) / 6;
            d := 1 / c; a[n,n] := -d;
            for i := 1 step 1 until n-1 do
               begin a[i,n] := a[n,i] := d * i;
                  a[i,i] := d * (c - i ^ 2);
                  for j := 1 step 1 until i-1 do
                     a[i,j] := a[j,i] := - d * i * j
               end
         end testmatr;

         procedure invert 140(n, eps, out) dataresult:(a);
            comment invert matrix (CACM, Algorithm 140);
            value n, eps; real eps; integer n; array a; label out;
         begin real q; integer i, j, k;
            for i := 1 step 1 until n do
               begin
                  if abs(a[i,i]) <= eps then go to out;
                  q := 1 / a[i,i]; a[i,i] := 1;
                  for k := 1 step 1 until n do a[i,k] := a[i,k] * q;
                  for j := 1 step 1 until n do
                     if i != j then
                        begin q := a[j,i]; a[j,i] := 0;
                           for k := 1 step 1 until n do
                              a[j,k] := a[j,k] - q * a[i,k]
                        end j
               end i
         end invert 140;

         procedure print matrix(name, n, a);
            value n; string name; integer n; array a;
         begin integer i, j;
            outstring(1, "Matrix ");
            outstring(1, name);
            outstring(1, ":\n");
            for i := 1 step 1 until n do
               begin for j := 1 step 1 until n do
                  outreal(1, if abs(a[i,j]) < #-12 then 0 else a[i,j]);
                  outchar(1, "\n", 1)
               end i
         end print matrix;

         comment n is order of matrices;
         integer n;
         n := 5;

         outstring (1, "Second example\n");

         begin array A, B, C[1:n,1:n]; integer i, j, k;
            comment create test matrix A;
            testmatr(n, A);
            print matrix("A", n, A);
            comment B := inv(A);
            testmatr(n, B);
            invert 140(n, epsilon, sing, B);
            go to skip;
sing:       fault("Matrix is singular", n);
skip:       print matrix("B = inv(A)", n, B);
            comment C := A * B using GPS;
            i := GPS(i, 1.0, C[1,1], 0.0) * GPS(i, (n-1) * GPS(j, (n-1)
               * GPS(k, n, C[i,j], C[i,j] + A[i,k] * B[k,j]), C[i,j+1],
               0.0), C[i+1,1], 0.0);
            print matrix("C = A * B", n, C)
         end
      end of second example

end
