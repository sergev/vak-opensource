      subroutine pval(res,arg,b,n)
C...subroutine     pval      01/09/73
C...subroutine     pval      01/09/78
      dimension b(n)
      res=0.
      j=n
 10   if(j.le.0) return
      res=res*arg+b(j)
      j=j-1
      go to 10
      end
