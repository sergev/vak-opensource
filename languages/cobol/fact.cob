OCOBOL*> ***************************************************************
      *> Program to find range and domain of FUNCTION FACTORIAL
       identification division.
       program-id. fact.

       data division.
       working-storage section.
       01 ind                  pic 99.
       01 z-ind                pic z9.
       01 result               pic 9(18).
       01 pretty-result        pic z(17)9.

      *> ***************************************************************
       procedure division.
       perform varying ind from 0 by 1 until ind > 21
           add zero to function factorial(ind) giving result
               on size error
                   display
                       "overflow at " ind ", result undefined: "
                       function factorial(ind)
                   end-display
               not on size error
                   move ind to z-ind
                   move result to pretty-result
                   display
                       "factorial(" z-ind ") = " pretty-result
                   end-display
           end-add
       end-perform

       goback.
       end program fact.
