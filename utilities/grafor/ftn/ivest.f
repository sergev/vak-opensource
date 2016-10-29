      function ivest (a,b,eps)
C        ivest  01/06/75  01/01/80
      ivest=0
      if(abs(a-b).le.eps) return
      ivest=1
      if(a.lt.b) ivest=-1
      return
      end
