       subroutine wrstr(store)
C      subroutine wrstr  01/06/75
       common /gfgel/gf(28)
       dimension store(28)
       do 1 i=1,28
       store(i)=gf(i)
  1    continue
       return
       end
