       subroutine drfram(lx,ly,x,y,funx,funy)
C...01/09/78
	external funx,funy
       dimension x(lx),y(ly),ishag(4),jshag(4),mepa(4)
       data ishag/1,0,-1,0/,jshag/0,1,0,-1/,ix,iy/1,1/
       mepa(1)=lx-1
       mepa(3)=mepa(1)
       mepa(2)=ly-1
       mepa(4)=mepa(2)
       call tmf(funx(x(1),y(1)),funy(x(1),y(1)),xf,yf)
       call move(xf,yf,0)
       do 1 k=1,4
       is=ishag(k)
       js=jshag(k)
      metp=mepa(k)
       do 1 i=1,metp
       ix=ix+is
       iy=iy+js
       call tmf(funx(x(ix),y(iy)),funy(x(ix),y(iy)),xf,yf)
1      call move(xf,yf,1)
       return
       end
