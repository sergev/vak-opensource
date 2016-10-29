      subroutine bars(y,yp,n,w,ihat,np1)
C   subroutine     bars      01/09/78
      dimension y(1),yp(1)
      x=0.
      do 1 i=1,n
      h=y(i)
      sh=yp(i)
      call bar(x,0.,h,w,sh,ihat,np1)
      x=x+w
  1   continue
      return
      end
