 PROGRAM QUINE
   CHARACTER(LEN=*), PARAMETER :: Quote = Char(39)
   CHARACTER(LEN=86), DIMENSION(11) :: Source
   INTEGER :: i ; CHARACTER(LEN=2) :: Line
   Source( 1) = 'PROGRAM QUINE'
   Source( 2) = '  CHARACTER(LEN=*), PARAMETER :: Quote = Char(39)'
   Source( 3) = '  CHARACTER(LEN=86), DIMENSION(11) :: Source'
   Source( 4) = '  INTEGER :: i ; CHARACTER(LEN=2) :: Line'
   Source( 5) = '  DO i=1, 4 ; WRITE(*,*) TRIM(Source(i)) ; END DO'
   Source( 6) = '  DO i=1, 11'
   Source( 7) = '    WRITE(Line,"(i2)") i'
   Source( 8) = '    WRITE(*,*) "  Source(" // Line // ") = " // Quote // TRIM(Source(i)) // Quote'
   Source( 9) = '  END DO'
   Source(10) = '  DO i=5, 11 ; WRITE(*,*) TRIM(Source(i)) ; END DO'
   Source(11) = 'END PROGRAM QUINE'
   DO i=1, 4 ; WRITE(*,*) TRIM(Source(i)) ; END DO
   DO i=1, 11
     WRITE(Line,"(i2)") i
     WRITE(*,*) "  Source(" // Line // ") = " // Quote // TRIM(Source(i)) // Quote
   END DO
   DO i=5, 11 ; WRITE(*,*) TRIM(Source(i)) ; END DO
 END PROGRAM QUINE
