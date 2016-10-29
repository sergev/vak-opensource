      subroutine prsp(vx,vy,vz,x,y,a,llx,lly,lxinit,lxend,lyinit,lyend,
     *xf1,yf1,rmax,rmin,nn)
C...subroutine prsp 01/09/78
       common /gfxy/ xf,yf,r1,xlast,ylast
      common /gfvp/ v1,v2,v3,xr,yr,infin
      dimension rmax(nn),rmin(nn)
       dimension x(llx),y(lly),xf1(1),yf1(llx),a(llx,lly)
       common /gfkj/ kj,n
       common/gftab/ irdb(5),rdb(17)
      v1=vx
      v2=vy
      v3=vz
      lxx=llx
      lyy=lly
        lxin=lxinit
        lxe=lxend
        lyin=lyinit
        lye=lyend
         n=nn
         r1=(n-1) / rdb(9)
       if(lxin.lt.0)   go to 51
      do 135 mm=1,n
       rmax(mm)=0.
135   rmin(mm)=1.
       go to   53
51    lxin=-lxin
 53     kj=1
       if(lyin.ge.0)   go to 56
      lyin=-lyin
       kj=-1
56    continue
             xmin=x(lxin)
             ymin=y(lyin)
             xmax=x(lxe)
             ymax=y(lye)
C   --------------------------------------------------------------------
C
         if (v1.le.xmin)  go to 11
      if(v1.ge.xmax)  go to 12
      if(v2.le.ymin)  go to 26
      if (v2.ge.ymax)   go to 27
C
C     this is point v inside surface
       do 1 mv=lxin,lxe
      if(v1.le.x(mv))  go to 3
  1   continue
   3  v1=x(mv)
       if(mv.ge.lxe)   go to 28
       do 2 mg=lyin,lye
      if(v2.le.y(mg))   go to 4
  2   continue
  4   v2 = y (mg)
       if(mg.ge.lye)   go to 27
45    continue
      call cornl(mv,lxe,mg,lye,mg,1,mv,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
      call cornl(lxin,mv,mg,lye,mg,2,mv,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       call cornl(lxin,mv,lyin,mg,mg,2,mv,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       call cornl(mv,lxe,lyin,mg,mg,1,mv,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       go to   30
C
C     this is south
26     do 7 m=lxin,lxe
      if(v1.le.x(m))   go to 8
  7   continue
  8   v1=x(m)
       if(m.ge.lxe)   go to 22
       call cornl(lxin,m,lyin,lye,lyin,2,m,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       call cornl(m,lxe,lyin,lye,lyin,1,m,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
      go to 30
C
C     this is   north
27     do 31 m=lxin,lxe
      if(v1.le.x(m))   go to 32
  31  continue
 32   v1=x(m)
       if(m.ge.lxe)   go to 23
       call cornl(lxin,m,lyin,lye,lye,2,m,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       call cornl(m,lxe,lyin,lye,lye,1,m,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
      go to 30
  11  if (v2.le.ymin)  go to 20
      if (v2.ge.ymax)  go to 21
C     this is west
       do 5 m=lyin,lye
      if(v2.le.y(m))  go to 6
   5  continue
6      v2=y(m)
       if(m.ge.lye)   go to 21
       call cornl(lxin,lxe,lyin,m,m,1,lxin,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       call cornl(lxin,lxe,m,lye,m,1,lxin,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
      go to 30
C     this is north-west
21    continue
       call cornl(lxin,lxe,lyin,lye,lye,1,lxin,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
      go to 30
C     this is sowth - west
20    continue
       call cornl(lxin,lxe,lyin,lye,lyin,1,lxin,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
      go to 30
  12  if (v2.le.ymin)   go to 22
      if (v2.ge.ymax)  go to 23
C     this is east
 28    do 33 m=lyin,lye
      if (v2.le.y(m))   go to 34
  33  continue
  34  v2=y(m)
       if(m.ge.lye)   go to 23
       call cornl(lxin,lxe,lyin,m,m,2,lxe,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       call cornl(lxin,lxe,m,lye,m,2,lxe,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
       go to   30
C     this is sowth-east
22    continue
       call cornl(lxin,lxe,lyin,lye,lyin,2,lxe,1,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
      go to 30
C     this is north-east
23    continue
       call cornl(lxin,lxe,lyin,lye,lye,2,lxe,2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
 30   return
      end
