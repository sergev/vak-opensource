      subroutine izflin(z,m,n,x,y,ziz,l,nx,ny,kx,ky,nf,xi,yi,nl)
      dimension nx(nf),ny(nf),kx(nf),ky(nf)
      dimension z(m,n),xi(nl),yi(nl)
      dimension x(m),y(n)
      dimension ziz(l)
      call raise (z,m ,n ,zmi)
      do 1 i=1,nf
      do 1 num=1,l
      call izlin(z,m,n,x,y,zmi,ziz(num),num,nx(i),ny(i),
     *kx(i),ky(i),xi,yi,nl)
    1 continue
      call lower (z,m,n,zmi)
      return
      end
