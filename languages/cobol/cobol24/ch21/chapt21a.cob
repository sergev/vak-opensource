000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt21a.
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000147 Data Division.
000148 Working-Storage Section.
000157 01  Current-Date-Group.
000167     03  Todays-Date.
000177         05  Today-YYYY       Pic 9(4).
000187         05  Today-MM         Pic 9(2).
000197         05  Today-DD         Pic 9(2).
000207     03  Time-Now.
000217         05  Time-Hour        Pic 99.
000227         05  Time-Minutes     Pic 99.
000237         05  Time-Seconds     Pic 99.
000247         05  Time-Hundredths  Pic 99.
000257     03  GMT-Offset.
000258         05  GMT-Direction    Pic X.
000259         05  GMT-Hours        Pic 99.
000260         05  GMT-Minutes      Pic 99.
000390 Procedure Division.
000391 Chapt21a.
000401     Move Function Current-Date To Current-Date-Group
000402     Display "Today = " Todays-Date
000403     Display "Time  = " Time-Now
000404     Display "GMT offset = " GMT-Offset
000411     Stop Run
000421     .