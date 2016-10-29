       function modgf(j)
C      function modgf 01/06/75
       common/gfgel/gf(28)
       k=gf(9)
       i=iabs(k)
       if(j.ne.0) goto 1
       modgf=1
       if(i.ne.1) modgf=11
       modgf=isign(modgf,k)
       return
  1    if(k.lt.0) i=12-i
       modgf=1
       if(gf(i)) 3,4,2
  2    modgf=2
       return
 3     modgf=3
       return
 4     if(abs(gf(i+3)).ge.1e-6) return
       if(abs(gf(i+4)).ge.1e-6) return
       modgf=0
       return
       end
