       function hcind(x,ist,ifn,el)
      dimension x(1)
      i1=ifn-1
      do 1 i=ist,i1
      if(el-x(i))1,3,2
    2 if(el.ge.x(i+1))goto 1
   3  i1=i
      goto 4
    1 continue
    4 hcind=i1
      return
      end
