 Program QUINE                                      
    character*51::code(16),li                       
    integer::lin,qn,lqn                             
    data code/&                                     
       'Program QUINE                                      ',&
       '   character*51::code(16),li                       ',&
       '   integer::lin,qn,lqn                             ',&
       '   data code/&                                     ',&
       '   do lin=1,4;print*,code(lin);enddo               ',&
       '   do lin=1,15                                     ',&
       '      li=code(lin);qn=index(li,'''''''');lqn=0     ',&
       '      do while(qn/=0)                              ',&
       '         li=li(:lqn+qn-1)//''''''''//li(lqn+qn:50) ',&
       '         lqn=lqn+qn+1;qn=index(li(lqn+1:),'''''''')',&
       '      enddo                                        ',&
       '      print*,''      '''''',li,'''''',&''          ',&
       '   enddo                                           ',&
       '   print*,''      '''''',code(16),''''''/''        ',&
       '   do lin=5,16;print*,code(lin);enddo              ',&
       'end Program QUINE                                  '/
    do lin=1,4;print*,code(lin);enddo               
    do lin=1,15                                     
       li=code(lin);qn=index(li,'''');lqn=0         
       do while(qn/=0)                              
          li=li(:lqn+qn-1)//''''//li(lqn+qn:50)     
          lqn=lqn+qn+1;qn=index(li(lqn+1:),'''')    
       enddo                                        
       print*,'      ''',li,''',&'                  
    enddo                                           
    print*,'      ''',code(16),'''/'                
    do lin=5,16;print*,code(lin);enddo              
 end Program QUINE                                  
