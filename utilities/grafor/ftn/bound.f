      subroutine bound(z,m,n,x,y,nx,ny,kx,ky,nf,xmi,xma,ymi,yma)
      dimension z(m,n),x(m),y(n),nx(nf),ny(nf),kx(nf),ky(nf)
      common /gfbet/kt,istep,size
      i=nx(1)
      j=ny(1)
      if (kt.ne.0)go to 5
      xmi=x(i)
      xma=xmi
      ymi=y(j)
      yma=ymi
      do 4 l=1,nf
      nxn=nx(l)
      nyn=ny(l)
      kxk=kx(l)
      kyk=ky(l)
C
      do 2 i=nxn,kxk
      xt=x(i)
      if (xmi.gt.xt) xmi=xt
      if (xt.gt.xma) xma=xt
  2   continue
      do 3 j=nyn,kyk
      yt=y(j)
      if (ymi.gt.yt) ymi=yt
      if (yt.gt.yma) yma=yt
  3   continue
  4   continue
      return
  5   continue
C*****xmi= atdx (z(1,1),x(1),y(1))
      xmi= atdx (z(i,j),x(i),y(j))
      xma=xmi
C*****ymi= atdy (z(1,1),x(1),y(1))
      ymi= atdy (z(i,j),x(i),y(j))
      yma=ymi
      do 10 l=1,nf
      nxn=nx(l)
      nyn=ny(l)
      kxk=kx(l)
      kyk=ky(l)
C
      do 10 i=nxn,kxk
      do 10 j=nyn,kyk
      xt= atdx (z(i,j),x(i),y(j))
      yt= atdy (z(i,j),x(i),y(j))
      if (xmi.gt.xt) xmi=xt
      if (ymi.gt.yt) ymi=yt
      if (xt.gt.xma) xma=xt
      if (yt.gt.yma) yma=yt
  10  continue
      return
      end
