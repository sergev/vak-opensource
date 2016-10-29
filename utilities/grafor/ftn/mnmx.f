      subroutine mnmx(vx,vy,vz,x,y,a,llx,lly,lxini,lxe,lyin,lye,s)
      dimension x(llx),y(lly),a(llx,lly)
      common/gfvp/ v1,v2,v3,xr,yr,infin
      v1=vx
      v2=vy
      v3=vz
      lxin=lxini
      if(lxin.ge.0)   go to 55
      lxin=-lxin
      k=-1
      go to   56
  55  k=1
56    continue
      xmin=x(lxin)
      ymin=y(lyin)
      xmax=x(lxe)
      ymax=y(lye)
C
         if (v1.le.xmin)   goto 11
      if(v1.ge.xmax)   go to 12
      if(v2.le.ymin)   go to 26
      if (v2.ge.ymax)   go to 27
C
C     this is point v inside surface
      do 1 mv=lxin,lxe
      if(v1.le.x(mv))  go to 3
  1   continue
   3  v1=x(mv)
      if(mv.ge.lxe)  go to 28
      do 2 mg=lyin,lye
      if(v2.le.y(mg))   go to 4
  2   continue
  4   v2 = y (mg)
      if(mg.le.lye)   goto  27
C
C     this is south
26    do 7 m=lxin,lxe
      if(v1.le.x(m))   go to 8
  7   continue
  8   v1=x(m)
      go to   30
C
C     this is   north
27    do 31 m=lxin,lxe
      if(v1.le.x(m))   go to 32
 31    continue
 32   v1=x(m)
      go to 30
11    if(v2.le.ymin)   goto 30
      if(v2.ge.ymax)   goto 30
C     this is west
       do 5 m=lyin,lye
      if(v2.le.y(m))   go to 6
   5  continue
6      v2=y(m)
      go to 30
12     if(v2.le.ymin)  go to 30
      if(v2.ge.ymax)   go to 30
C     this is east
 28    do 33 m=lyin,lye
      if (v2.le.y(m))   go to 34
  33  continue
  34  v2=y(m)
       go to   30
C   --------------------------------------------------------------------
30    jbg=0
       do 99 j=lyin,lye
       do 99 i=lxin,lxe
      call map(x(i),y(j),a(i,j))
             if(infin.gt.0)   go to 99
      jbg=jbg+1
       if(jbg.gt.1)   go to 71
      xmin=xr
      xmax = xr
      ymin = yr
      ymax = yr
      go to  99
71    continue
      if (xmin.gt.xr)  xmin=xr
      if (xmax.lt.xr)   xmax=xr
      if (ymin.gt.yr)  ymin=yr
      if (ymax.lt.yr)  ymax=yr
   99 continue
       if(k.ge.0)   goto 52
             if(xmn.gt.xmin)   xmn=xmin
      if(xmx.lt.xmax)   xmx=xmax
             if(ymn.gt.ymin)   ymn=ymin
             if(ymx.lt.ymax)   ymx=ymax
             go to   53
C  ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
52    continue
       xmn=xmin
       xmx=xmax
       ymn=ymin
       ymx=ymax
53    call limits(xmn,xmx,ymn,ymx)
       s=(xmx-xmn)/(ymx-ymn)
      return
      end
