      subroutine rccntc(cn,c,r1,r2)
C......ha t  pa  yc  okpy hocte  c  a ahh m  ehtpom, kaca8  xc
C....... py"o   a ahho  okpy hoct
      dimension cn(2),c(3)
      d=sqrt((c(1)-cn(1))**2+(c(2)-cn(2))**2)
      if(abs(d).lt.1.e-6) return
      r1=d+c(3)
      r2=d-c(3)
      return
       end
