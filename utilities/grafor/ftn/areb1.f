      subroutine areb1(th0,thf,g,th02,thf2)
C     subroutine    areb1     01/04/76
      th01=th0*0.01745329
      thf1=thf*0.01745329
      p=0.
      th=th0
      th1=th01
   15 if(th-90.) 106,107,108
  106 z5=sin(th1)*g/cos(th1)
      th2=atan(z5)/0.01745329
      go to 115
  107 th2=90.
      go to 115
  108 if(th-180.) 109,110,111
  109 t1=3.141593-th1
      z5=sin(t1)*g/cos(t1)
      th2=180.-atan(z5)/0.01745329
      go to 115
  110 th2=180.
      go to 115
  111 if(th-270.) 112,113,114
  112 t1=th1-3.141593
      z5=sin(t1)*g/cos(t1)
      th2=180.+atan(z5)/0.01745329
      go to 115
  113 th2=270.
      go to 115
  114 t1=6.28318531-th1
      z5=sin(t1)*g/cos(t1)
      th2=360.-atan(z5)/0.01745329
  115 if(p) 3,116,117
  116 th02=th2
      th=thf
      th1=thf1
      p=1.
      go to 15
  117 thf2=th2
    3 return
      end
