       subroutine dralim(lx,ly,x,y,funx,funy,s)
C...01/09/78
       dimension x(lx),y(ly)
	external funx,funy
       common/drlm/xmin,xmax,ymin,ymax
       xmin=funx(x(1),y(1))
       xmax=xmin
       ymin=funy(x(1),y(1))
       ymax=ymin
       i=0
       lym1=ly-1
      lxm1=1
1      do 2 ix=1,lx,lxm1
       do 2 iy=1,ly,lym1
       rx=funx(x(ix),y(iy))
       ry=funy(x(ix),y(iy))
       if(rx-xmin) 3,4,5
3      xmin=rx
       go to 4
   5   if(rx.gt.xmax) xmax=rx
4      if(ry-ymin) 7,2,9
7      ymin=ry
       go to 2
   9   if(ry.gt.ymax) ymax=ry
2      continue
       if(i.ne.0) go to 11
       i=1
       lym1=1
       lxm1=lx-1
       go to 1
11     s=(ymax-ymin)/(xmax-xmin)
       return
       end
