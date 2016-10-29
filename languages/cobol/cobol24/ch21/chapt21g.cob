000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt21g.
000040* Convert Local Time To Gmt
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
000198     03  Todays-Date-N Redefines Todays-Date Pic 9(8).
000207     03  Time-Now.
000217         05  Time-Hour        Pic 99.
000227         05  Time-Minutes     Pic 99.
000237         05  Time-Seconds     Pic 99.
000247         05  Time-Hundredths  Pic 99.
000257     03  GMT-Offset.
000258         05  GMT-Direction    Pic X.
000259         05  GMT-Hours        Pic 99.
000260         05  GMT-Minutes      Pic 99.
000270 01  Display-Date.
000280     03  Today-MM             Pic 9(2).
000290     03  Filler               Pic X Value "/".
000300     03  Today-DD             Pic 9(2).
000310     03  Filler               Pic X Value "/".
000320     03  Today-YYYY           Pic 9(4).
000321 01  Display-Time.
000322     03  Time-Hour            Pic 99.
000323     03  Filler               Pic X Value ":".
000324     03  Time-Minutes         Pic 99.
000325     03  Filler               Pic X Value ":".
000326     03  Time-Seconds         Pic 99.
000330 01  Total-Seconds            Pic 9(15) Value Zeros.
000340 01  Work-Number              Pic 9(15) Value Zeros.
000350 01  Work-Remainder           Pic 9(15) Value Zeros.
000390 01  GMT-Offset               Pic 9(15) Value Zeros.
000391 Procedure Division.
000392 Chapt21g.
000401     Move Function Current-Date To Current-Date-Group
000411* Convert Today To Seconds
000421     Compute Work-Number =
000422             Function Integer-Of-Date (Todays-Date-N)
000431     Compute Total-Seconds = (Work-Number * 86400) +
000441                             (Time-Hour Of Time-Now * 3600) +
000451                             (Time-Minutes Of Time-Now * 60) +
000461                             Time-Seconds Of Time-Now
000471     Compute Work-Number = (GMT-Hours * 3600) +
000481                           (GMT-Minutes * 60)
000491* Remember We Need To Change By The Opposite Of The Direction From Gmt
000492     If GMT-Direction = "+"
000501        Subtract Work-Number From Total-Seconds
000511     Else
000521        Add Work-Number To Total-Seconds
000531     End-If
000541* Convert The Time In Seconds Back To A Date And Time
000551     Divide Total-Seconds By 86400 Giving Work-Number
000561                             Remainder Work-Remainder
000571     Compute Todays-Date-N =
000572             Function Date-Of-Integer (Work-Number)
000581     Divide Work-Remainder By 3600 Giving Time-Hour Of Time-Now
000591                              Remainder Work-Number
000601     Divide Work-Number By 60 Giving Time-Minutes Of Time-Now
000611                              Remainder Time-Seconds Of Time-Now
000621     Move Corresponding Todays-Date To Display-Date
000631     Move Corresponding Time-Now To Display-Time
000641     Display "Current GMT " Display-Date " " Display-Time
000651     Stop Run
000661     .