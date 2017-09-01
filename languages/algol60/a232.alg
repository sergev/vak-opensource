begin

  comment This example doesn't work with marst 2.1 because of incorrect
     passing a formal parameter called by name to other procedure (but
     it does work with marst 2.2). This example program was reported by
     Paulo Barreto <paulo.barreto@terra.com.br>;

  comment
    ALGORITHM 232
    HEAPSORT
    J. W. J. Williams (Received 1 Oct. 1963 and revised 15 Feb. 1964)
    Elliott Bros. (London) Ltd., Borehamwood, Herts, England;

  comment The following procedures are related to TREESORT
      [R. W. Floyd, Alg. 113, Comm. ACM 5 (Aug. 1962), 434, and
      A. F. Kaupe, Jr., Alg. 143 and and 144, Comm. ACM 5 (Dec. 1962),
      604] but avoid the use of pointers and so preserve storage space.
      All the procedures operate on single word items, stored as
      elements 1 to n of the array A. The elements are normally so
      arranged that A[i] <= A[j] for 2 <= j <= n, i = j % 2. Such an
      arrangement will be called a heap. A[1] is always the least
      element of the heap.
        The procedure SETHEAP arranges n elements as a heap, INHEAP
      adds a new element to an existing heap, OUTHEAP extracts the
      least element from a heap, and SWOPHEAP is effectively the
      result of INHEAP followed by OUTHEAP. In all cases the array A
      contains elements arranged as a heap on exit.
        SWOPHEAP is essentially the same as the tournament sort
      described by K. E. Iverson - A Programming Language, 1962,
      pp. 223--226 - which is a top to bottom method, but it uses an
      improved storage allocation and initialisation. INHEAP resembles
      TREESORT in being a bottom to top method. HEAPSORT can thus be
      considered as a marriage of these two methods.
        The procedures may be used for replacement-selection
      sorting, for sorting the elements of an array, or for choosing
      the current minimum of any set of items to which new items are
      added from time to time. The procedures are the more useful
      because the active elements of the array are maintained densely
      packed, as elements A[1] to A[n];

  procedure SWOPHEAP(A, n, in, out);
    value in, n;
    integer n;
    real in, out;
    real array A;
    comment SWOPHEAP is given an array A, elements A[1] to A[n]
        forming a heap, n >= 0. SWOPHEAP effectively adds the element in
        to the heap, extracts ans assigns to out the value of the least
        member of the resulting set, and leaves the remaining elements
        in a heap of the original size. In this process elements 1 to
        (n + 1) of the array A may be disturbed. The maximum number of
        repetitions of the cycle labeled scan is log_2 n;
  begin
    integer i, j;
    real temp, temp 1;
    if in <= A[1] then
      out := 1
    else begin
      i := 1;
      A[n + 1] := in; comment this last statement is only
        necessary in case j = n at some stage, or n = 0;
      out := A[1];
    scan:
      j := i + i;
      if j <= n then begin
        temp := A[j];
        temp 1 := A[j + 1];
        if temp 1 < temp then begin
          temp := temp 1;
          j := j + 1
        end;
        if temp < in then begin
          A[i] := temp;
          i := j;
          go to scan
        end
      end;
      A[i] := in
    end
  end SWOPHEAP;

  procedure INHEAP(A, n, in);
    value in;
    integer n;
    real in;
    real array A;
    comment INHEAP is given an array A, elements A[1] to A[n]
        forming a heap and n >= 0. INHEAP adds the element in
        to the heap and adjusts n accordingly. The cycle labeled scan
        may be repeated log_2 n times, but on average is repeated twice
        only;
  begin
    integer i, j;
    i := n := n + 1;
  scan:
    if i > 1 then begin
      j := i % 2;
      if in < A[j] then begin
        A[i] := A[j];
        i := j;
        go to scan
      end
    end;
    A[i] := in
  end INHEAP;

  procedure OUTHEAP(A, n, out);
    integer n;
    real out;
    real array A;
    comment given array A, elements 1 to n of which form a heap,
        n >= 1, OUTHEAP assigns to out the value of A[1], the least
        member of the heap, and rearranges the remaining members as
        elements 1 to n - 1 of A. Also, n is adjusted accordingly;
  begin
    SWOPHEAP(A, n - 1, A[n], out);
    n := n - 1
  end OUTHEAP;

  procedure SETHEAP(A, n);
    value n;
    integer n;
    real array A;
    comment SETHEAP rearranges the elements A[1] to A[n] to form
      a heap;
  begin
    integer j;
    j := 1;
  L:
    INHEAP(A, j, A[j + 1]);
    if j < n then go to L
  end SETHEAP;

  procedure HEAPSORT TEST(n);
    value n;
    integer n;
    comment HEAPSORT TEST tests the implementation on a random array
      of n elements. This procedure is not part of the original
      algorithm definition, and is only provided for completeness;
  begin
    integer i;
    real out;
    real array A[1:n];
    comment create a test array of n elements;
    for i := 1 step 1 until n do begin
      A[i] := random
    end;
    SETHEAP(A, n);
    
    comment CAVEAT - the following loop does not work;
    i := n;
  loop:
    OUTHEAP(A, i, out);
    A[i + 1] := out; comment remember that i is decremented by OUTHEAP;
    if i > 1 then go to loop;

    comment HOWEVER, the following code does work;
    for i := n step -1 until 2 do begin
      SWOPHEAP(A, i - 1, A[i], out);
      A[i] := out
    end;
    
    comment now check if A is sorted in descending order;
    for i := 2 step 1 until n do begin
      if A[i] > A[i - 1] then begin
        outstring(1, "HEAPSORT error!\n");
        go to exit
      end
    end;
    outstring(1, "HEAPSORT implementation is correct!\n");
  exit:
  end HEAPSORT TEST;

  real procedure random;
  begin
    inline("my_dsa.retval.u.real_val = rand();")
  end random;

  comment main program;
  HEAPSORT TEST(10)

end
