      function deriv5 (dx,y,n,i)
C...function       deriv5    01 08 74
      dimension y(n)
      if(n-4) 15,9,10
   15 if(n-2) 17,16,8
   16 a=(y(2)-y(1))/dx
      go to 17
    8 if(i-2) 11,12,13
   11 a=-18*y(i)+24*y(i+1)-6*y(i+2)
      go to 7
   12 a=-6*y(i-1)+6*y(i)
      go to 7
   13 a=6*y(i-2)-24*y(i-1)+18*y(i)
      go to 7
    9 if(i-2) 2,3,14
   14 if(i-4) 13,4,4
   10 if(i-2) 2,3,1
    1 if(n-1-i) 4,5,6
    2 a=4*y(4)-18*y(3)+36*y(2)-22*y(1)
      go to 7
    3 a=-2*y(4)+12*y(3)-6*y(2)-4*y(1)
      go to 7
    4 a=22*y(i)-36*y(i-1)+18*y(i-2)-4*y(i-3)
      go to 7
    5 a=3*y(i+1)+11*y(i)-21*y(i-1)+9*y(i-2)-2*y(i-3)
      go to 7
    6 a=-y(i+2)+8*y(i+1)-8*y(i-1)+y(i-2)
    7 deriv5=a*0.083333/dx
   17 return
      end
