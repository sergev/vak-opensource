      subroutine ccntl(cn,l,c)
C       octpoeh e okpy hoct  (c)  o  a ahhomy  ehtpy (p)   kacate  ho
C       k okpy hoct  (l)
      real cn(2),l(3),c(3)
      s=sqrt(l(1)*l(1)+l(2)*l(2))
      c(1)=cn(1)
      c(2)=cn(2)
       c(3)=abs(l(1)*cn(1)+l(2)*cn(2)+l(3))/s
      return
      end
