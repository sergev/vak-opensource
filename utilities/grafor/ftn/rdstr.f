       subroutine rdstr(store)
C      subroutine rdstr 01/06/75
       common/gfgel/gf(28)
       dimension store(28)
       do 1 i=1,28
       gf(i)=store(i)
  1    continue
       return
       end
