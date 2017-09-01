begin
   comment This example fails with marst 2.3 because of incorrect
      generation of references to procedure blocks in some rare cases
      (however it works with marst 2.4). This program was provided by
      Jan van Katwijk <J.vanKatwijk@ITS.TUDelft.NL>;
   comment Note that due to very deep recursion at run-time the stack
      size should be large enough (about 8 Mb);
   integer i;
   integer procedure A (k, x1, x2, x3, x4, x5);
   value k; integer k, x1, x2, x3, x4, x5;
   begin
      integer procedure B;
      begin
         k := k - 1;
         A := B := A (k, B, x1, x2, x3, x4)
      end;
      if k <= 0 then A := x4 + x5 else B
   end;
   for i := 1 step 1 until 14 do
   begin
      outinteger (1, A (i, 1, -1, -1 , 1 , 1));
      outstring (1, "\n")
   end
end
