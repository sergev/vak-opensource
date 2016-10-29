      subroutine map (x,y,z)
      common/gfvp/ v1,v2,v3,xr,yr,infin
      infin=0
      d1= v1*v1+v2*v2
      d= d1+v3*v3
      av=d-(x*v1+y*v2+z*v3)
      p=(v1-x)*(v1-x)+(v2-y)*(v2-y)+(v3-z)*(v3-z)
      csn=.1736
      difr=av - sqrt(d*p) * csn
      if(difr.lt.0)   go to 4
      r=d/av
      if (d1.eq.0)  go to 2
      xr = r*(y*v1-x*v2) / sqrt(d1)
      yr = (v3+r*(z-v3))*sqrt(d /d1)
      return
    2 xr = r*x
      yr = r*y
      return
 4    infin=1
      return
      end
