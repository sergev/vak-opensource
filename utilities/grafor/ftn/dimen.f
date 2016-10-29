      subroutine dimen(xs,ys,datt,slope)
C **      for h-10/mt             es-1022
C... to draw a dimension line with arrows and dimensions
C       xs,ys       x,y coord. of the sarting point of the dimension.
C       datt        numerical value drawn along the dimension line.
C       slope       angle which the dimension line forms with the posit.
C                   of the x-axis.
       integer sketa
      common /gftab/irdb(5),rdb(17)
       f=rdb(17)/rdb(1)
       theta=slope*0.01745329
       xa=xs
       ya=ys
       datt=abs(datt)
       xf=datt*cos(theta)+xs
       yf=datt*sin(theta)+ys
       call narrow(xs,ys,xf,yf,0.4*f,3)
       do 20  i=1,5
       sketa=i-1
       if(datt.le.-10.0**sketa) goto 30
  20   continue
       goto 50
  30   if(sketa.le.0) sketa=1
  50   sketa=0.5*sketa
       if(slope.gt.90.0)goto 400
       if(slope+90.0.ge.0) goto 500
 400   theta=theta+3.14159265
       xa=xf
       ya=yf
  500   xx=(datt/2.0-(0.4*f)*float(sketa))*cos(theta)+xa-(sin(theta)*f
     c *0.1)
        yy=(datt/2.0-(0.4*f)*float(sketa))*sin(theta)+ya+(cos(theta)*
     c f*0.1)
       alpha=theta*180.0/3.14159265
       call number(xx,yy,0.4*f,datt,2,alpha)
       return
       end
