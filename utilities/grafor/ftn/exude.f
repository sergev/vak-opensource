      subroutine exude(z,m,n,x,y,nx,ny,kx,ky,nf,mrka,mrki,kd,h,th)
      dimension nx(nf),ny(nf),ky(nf),kx(nf)
      dimension z(m,n),x(m),y(n)
      do 1 l=1,nf
      nxs=nx(l)+1
      nys=ny(l)+1
      kxs=kx(l)-1
      kys=ky(l)-1
      do 1 i=nxs,kxs
      do 1 j=nys,kys
      if((z(i,j).gt.z(i+1,j)).and.(z(i,j).gt.z(i-1,j))) go to 2
      if((z(i,j).lt.z(i+1,j)).and.(z(i,j).lt.z(i-1,j))) go to 3
      go to 1
    2 if((z(i,j).gt.z(i,j+1)).and.(z(i,j).gt.z(i,j-1)))go to 4
    3 if((z(i,j).lt.z(i,j+1)).and.(z(i,j).lt.z(i,j-1)))go to 5
      go to 1
  4   mrk=mrka
      go to 6
  5   mrk=mrki
  6   xs=atdx(z(i,j),x(i),y(j))
      ys=atdy(z(i,j),x(i),y(j))
      call token(xs,ys,mrk, z(i,j),kd,h,th)
    1 continue
      return
      end
