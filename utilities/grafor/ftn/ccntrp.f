       subroutine ccntrp(cn,p,c)
C........ octpoeh e okpy hoct   o  a ahhomy  ehtpy   to ke,
C........  e a e  ha okpy hoct
      dimension cn(2),p(2),c(3)
      c(1)=cn(1)
      c(2)=cn(2)
      c(3)=sqrt((cn(1)-p(1))**2+(cn(2)-p(2))**2)
      return
      end
