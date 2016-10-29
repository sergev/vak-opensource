      subroutine patch(xf1,yf1,rmax,rmin)
C...subroutine patch 01/09/78
      common /gfl/ l,l1,n5,i,ldedg,ldgj,jj,lyj
       common /gfxy/ xf,yf,r1,xlast,ylast
      common /gfb/ ibx,byi,kk,nob,lllx,xlsi,ylsi
      common /gftab/ irdb(5), rdb(17)
      dimension rmax(1),rmin(1)
      dimension xf1(1),yf1(1)
      fis(xf)=r1*(xf -rdb(7))+1.5
      rmat(ix)=(float(ix)-1.) / r1+rdb(7)
      l =0
      l1=0
      n5=0
      if(kk.le.0)   n5=1
         ixf=fis(xf)
      yfi = yf
C
C
      if(rmin(ixf).le.rmax(ixf))    go to 103
      call linint (ibx,byi,ixf,yfi,rmax,rmin)
      n5=0
      go to 254
C
  103 if (yfi-rmax(ixf))     120,515,100
100   if(byi-rmax(ibx))   102,122,122
  515 if (rmax(ixf).ne.rmin(ixf))    go to 512
      if(byi.ge.rmax(ibx))    go to 122
      if (byi-rmin(ibx)) 122,122,102
102   call search(ixf,yfi,ibx,byi,xf1,yf1,rmax,rmin)
      if (l)   190,180,190
512   if(byi.gt.rmax(ibx))    go to 122
      if  (byi-rmin(ibx))     151,151,102
C -----------------------------------------------------------------
C
 120         if(yfi-rmin(ixf))      121,510,150
121   if(byi-rmin(ibx))   122,122,102
122   call bufl (ixf,yfi,ibx,byi,rmax,rmin)
C
254   if(l.ne.0)    go to 255
      call move (xf,yf,1)
         go to   180
  255 call move (xf1(i),yf1(i),1)
      if(jj.ne.lyj)   call move(xf,yf,0)
         go to   190
C
510   if (byi.le.rmin(ibx))    go to 122
      if (byi-rmax(ibx))     102,151,151
150   if(byi.ge.rmax(ibx))    go to 151
      if(byi-rmin(ibx))   151,151,300
  151 l1 = 1
      call search(ibx,byi,ixf,yfi,xf1,yf1,rmax,rmin)
      l1 = 0
      if(l)   190,180,190
C
C
  300 if (l.ne.0)   go to 311
      n5=0
 180  if(ldgj.lt.0)    go to 183
      if(ldedg.lt.0)    go to 182
      if(kk.gt.0)   go to 155
      ibx=ixf
      byi = yfi
      if(xf1(lllx+i).le.0)    go to 10
C  xf1(lllx+i)=1--ko" a he o pe e eha e e xf1(i).(t.e. h  o ha to ka
C     to"o cto   a he  o a a e e b pactp.)
             if(jj.le.lyj)    go to 12
             xf1(lllx+i)=0.
             go to   192
   12        nob=1
C nob| 1---  p  hak, to he  y et p cobat c   y y    kyco ek be y e"o
C      epe he"o  pe pa.(t.e.  o ctpoke,  o i).
             xlsi=xf
             ylsi=yf
              xf1(lllx+i)=0.
              go to   158
   10        continue
         ixf=fis(xf1(i))
      yfi = yf1(i)
      l =1
      if(jj-lyj)   171,172,171
172   ldedg=1
171   continue
      go to   103
182   ibx=fis(xlsi)
      byi=ylsi
      xlast=xlsi
      ylast=ylsi
      xf1(i)=xf
      yf1(i)=yf
      call move(xlsi,ylsi,0)
      return
183   ibx=ixf
      byi=yfi
      return
311   if(n5.eq.0)    go to 313
      n5=-1
      call bufl (ixf,yfi,ibx,byi,rmax,rmin)
313   n5=1
190   if(ldedg.le.0)    go to 192
      ibx=fis(xf1(i))
      byi=yf1(i)
      xlast=xf1(i)
      ylast=yf1(i)
      xlsi=xf
      ylsi=yf
      call move(xf1(i),yf1(i),0)
      go to   158
192   xlast=xf
      ylast=yf
 158  xf1(i)=xf
      yf1(i)=yf
  155 continue
      return
      end
