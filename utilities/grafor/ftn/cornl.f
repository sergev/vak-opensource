      subroutine cornl (lxi1,lx1,lyj1,ly1,k1,m1,k2,m2,
     *x,y,a,lxx,lyy,xf1,yf1,rmax,rmin)
C...subroutine cornl   01/09/78
      common /gfvp/ v1,v2,v3,xr,yr,infin
       common /gfkj/ kj,n
       common /gfxy/ xf,yf,r1,xlast,ylast
      common /gfl/ l,l1,n5,i,ldedg,ldgj,jj,lyj
      common /gfb/ ibx,byi,kk,nob,lllx,xlsi,ylsi
      common /gftab/ irdb(5),rdb(17)
      dimension rmax(1),rmin(1)
       dimension x(lxx),y(lyy),xf1(1),yf1(lxx),a(lxx,lyy)
      fis(xf)=r1* (xf - rdb(7)) + 1.5
C
       lxi=lxi1
       lx=lx1
       lyj=lyj1+1
       ly=ly1
             lllx=lxx
       if(kj.lt.0)    go to 15
       do 135 m=1,n
      rmax(m)=0.
  135 rmin(m)=1.
15     continue
C
C                         ,,leading edges,,
      ldedg=0
         ldgj=0
      kk=1
       lxl=lxx+lxx
             jbg=0
       do 100 i1=lxx,lxl
  100        xf1(i1)=0.
C   ---------------------------------------------------  - ------------
      do 80 k=1,3
      if(m1.ge.2)    go to 22
      go to (50,51,52),k
 50   jj=lxi+1
      go to  20
 51   jj=lxi
      go to   20
C
C
 22   go to (60,61,52),k
 60   jj=lx-1
      go to   20
 61   jj=lx
      go to   20
  52  if(m2.ge.2)  goto 54
      jj=lyj
      go to   25
 54   jj=ly-1
      go to   25
C
C        this is leading left edge
C
  20  call map (x(jj),y(k1),a(jj,k1))
             if(infin.le.0)   go to 101
             xf1(lxx+jj)=1.
             go to   80
  101        jbg=jbg+1
      call tmf(xr,yr,xf,yf)
      xf1(jj) = xf
      yf1(jj) = yf
         ixf=fis (xf)
      yfi = yf
             if(jbg.gt.1)   go to 76
  75  ibx = ixf
      byi = yfi
      call move (xf,yf,0)
      xlast=xf
      ylast=yf
      call patch(xf1,yf1,rmax,rmin)
      go to 80
76    call patch(xf1,yf1,rmax,rmin)
      ibx = ixf
      byi = yfi
      xlast=xf
      ylast=yf
      go to   80
C
C     this is leading front edge
  25  call map(x(k2),y(jj),a(k2,jj))
             if(infin.gt.0)   go to 80
              jbg=jbg+1
              xf1(lxx+k2)=0.
              call tmf(xr,yr,xf,yf)
              if(jbg.gt.1)   go to 108
              xf1(k2)=xf
              yf1(k2)=yf
              go to   75
  108         continue
         ixf=fis(xf)
      yfi = yf
      call patch(xf1,yf1,rmax,rmin)
      ibx = ixf
      byi = yfi
       xf1(k2)=xf
       yf1(k2)=yf
      xlast=xf
      ylast=yf
   80 continue
      lxi=lxi+1
             jcol=1
C  jcol=1---means there is not yet xlstj,ylstj.
             if(infin.gt.0)   go to  111
             jcol=0
      xlstj=xf
      ylstj=yf
  111        continue
      ldedg=0
      ldgj=0
      kk=0
C   ---------------------------------------------------  - ------------
C                            ,,patches,,
      do 155 jj=lyj,ly
      j=jj
      if(m2.ge.2)     j=ly-jj+lyj-1
C
  35  call map (x(k2),y(j),a(k2,j))
             irow=0
             if(infin.le.0)   go to 120
  121        irow=1
C   irow=1 --- means that a pen is absent yet in this matem. row.
             go to   123
  120        call tmf(xr,yr,xf,yf)
             if(jcol.le.0)   go to 124
             jcol=0
             xlstj=xf
             ylstj=yf
  124        continue
      ibx=fis(xlstj)
      byi=ylstj
      xlast=xlstj
      ylast=ylstj
      if(jj.le.lyj)   go to 27
      call move (xlstj,ylstj,0)
      ldgj=-1
      call patch(xf1,yf1,rmax,rmin)
      ldgj=0
 27   xlstj=xf
      ylstj=yf
      xf1(k2) = xf
      yf1(k2) = yf
       xlast = xf
       ylast = yf
  123        continue
C
              nob=0
C
      do 155 ii =lxi,lx
      if(jj.gt.lyj)   go to 43
      if(ii-lxi)   43,48,42
 42   i=ii
      if(m1.ge.2)    i=lx-ii+lxi-1
 45   call map (x(i),y(k1),a(i,k1))
             if(infin.le.0)   go to 131
             xf1(lxx+i)=1.
             ibx=fis(xlsi)
             byi=ylsi
             call move(xlsi,ylsi,0)
             xlast=xlsi
             ylast=ylsi
              nob=0
             go to   40
  131  call tmf(xr,yr,xf,yf)
             if(nob.le.0)   go to 133
             ixf=fis(xf)
             yfi=yf
             ibx=ixf
             byi=yfi
             xlast=xf
             ylast=yf
             call move(xf,yf,0)
              nob=0
  133        continue
      ldedg=-1
      call patch(xf1,yf1,rmax,rmin)
      ldedg=0
      go to   40
48    ldedg=1
 43   go to (36,37),m1
  36  i=ii
      go to 40
  37  i=lx-ii+lxi-1
C
  40  call map(x(i),y(j),a(i,j))
             if(infin.le.0)   go to 141
             if(jj.ne.lyj)   go to 155
      if(xf1(lxx+i).gt.0.)   goto 147
             ibx=fis(xf1(i))
             byi=yf1(i)
             xlast=xf1(i)
             ylast=yf1(i)
             go to   155
  147        nob=1
             go to   155
  141        continue
      call tmf (xr,yr,xf,yf)
             if(irow.le.0)   go to 143
             irow=0
             call move(xf,yf,0)
             ixf=fis(xf)
             yfi=yf
             ibx=ixf
             byi=yfi
             xlast=xf
             ylast=yf
  143        continue
      call patch(xf1,yf1,rmax,rmin)
      ldedg=0
155   continue
      return
      end
