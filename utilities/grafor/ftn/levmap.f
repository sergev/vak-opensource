      subroutine levmap(c,nc,f0,sp,sm,np)
      dimension c(1)
      do 1 i=1,np
    1 c(i)=f0+sp*i
      ns=np+1
      do 2 k=ns,nc
    2 c(k)=f0-sm*(k-ns)
      return
      end
