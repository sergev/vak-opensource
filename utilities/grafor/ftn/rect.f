      subroutine  rect (x,y,h,w,th)
      theta = th / 57.2958
      xs = sin (theta)
      xc = cos (theta)
      call move	(x,y,0)
      x1= x- h * xs
      y1= y+ h * xc
      call move	(x1,y1,1)
      x1= x1 + w* xc
      y1= y1 + w* xs
      call move	(x1,y1,1)
      x1= x + w	* xc
      y1= y + w	* xs
      call move	(x1,y1,1)
      call move	(x,y,1)
      return
      end
