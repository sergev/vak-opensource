      subroutine izolin(z,m,n,x,y,ziz,l,xi,yi,nl)
      dimension x(m),y(n)
      dimension z(m,n),xi(nl),yi(nl)
      dimension ziz(l)
      call raise (z,m ,n ,zmi)
      do 1 num=1,l
      call izlin(z,m,n,x,y,zmi,ziz(num),num,1,1,m,n,xi,yi,nl)
    1 continue
      call lower (z,m,n,zmi)
      return
      end
