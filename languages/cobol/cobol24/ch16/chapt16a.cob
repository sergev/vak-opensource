000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt16a.
000031* Relative File Access Example
000041 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status
000053       Cursor Is Cursor-Position.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Optional Relative-File Assign To "Relative.Dat"
000060            Organization Relative
000061            Access Dynamic
000062            Relative Key Relative-Key
000063            File Status Relative-Status.
000065 Data Division.
000066 File Section.
000067 Fd  Relative-File.
000068 01  Relative-Record.
000069     03  Relative-Data Pic X(20).
000098 Working-Storage Section.
000108 01  Relative-Key   Pic 99 Value Zeros.
000116 01  Relative-Status     Pic X(2) Value Spaces.
000240     88  Relative-Success Value "00" Thru "09".
000279 01  Keyboard-Status.
000280     03  Accept-Status Pic 9.
000281     03  Function-Key  Pic X.
000282         88  F1-Pressed Value X"01".
000283         88  F2-Pressed Value X"02".
000284         88  F3-Pressed Value X"03".
000285         88  F4-Pressed Value X"04".
000286         88  F5-Pressed Value X"05".
000287         88  F6-Pressed Value X"06".
000288         88  F7-Pressed Value X"07".
000289         88  F8-Pressed Value X"08".
000290     03  System-Use    Pic X.
000291 01  Cursor-Position.
000292     03  Cursor-Row    Pic 9(2) Value 1.
000293     03  Cursor-Column Pic 9(2) Value 1.
000294 01  Error-Message Pic X(50) Value Spaces.
000295 01  Table-Area.
000296     03  Table-Values.
000297         05  Filler Pic X(20) Value "Entry 1".
000298         05  Filler Pic X(20) Value "Entry 2".
000299         05  Filler Pic X(20) Value "Entry 3".
000300         05  Filler Pic X(20) Value "Entry 4".
000301         05  Filler Pic X(20) Value "Entry 5".
000302         05  Filler Pic X(20) Value "Entry 6".
000303         05  Filler Pic X(20) Value "Entry 7".
000304         05  Filler Pic X(20) Value "Entry 8".
000305         05  Filler Pic X(20) Value "Entry 9".
000306         05  Filler Pic X(20) Value "Entry 10".
000307     03  Load-Table Redefines Table-Values.
000308         05  Basic-Table Pic X(20) Occurs 10 Times.
000309 Screen Section.
000321 01  Data-Entry-Screen
000322     Blank Screen, Auto
000323     Foreground-Color Is 7,
000324     Background-Color Is 1.
000325*
000326     03  Screen-Literal-Group.
000327         05  Line 01 Column 25 Value "Relative File Example"
000328             Highlight Foreground-Color 4 Background-Color 1.
000331         05  Line 4  Column 01  Value "Current Relative Key: ".
000332         05  Line 5  Column 01  Value "Relative Data: ".
000347         05  Line 22 Column 01  Value "F1-Read Random Number".
000348         05  Line 22 Column 23  Value "F2-Start Number".
000349         05  Line 22 Column 56  Value "F3-Read Next Number".
000350         05  Line 23 Column 01  Value "F4-Delete Record".
000351         05  Line 23 Column 23  Value "F5-Write Record".
000352         05  Line 23 Column 56  Value "F6-Rewrite Record".
000353         05  Line 24 Column 01  Value "F7-Clear".
000354         05  Line 24 Column 23  Value "F8-Exit".
000355     03  Required-Reverse-Group Reverse-Video.
000356         05  Line 4 Column 23  Pic 9(2)  Using Relative-Key.
000357         05  Line 5 Column 16  Pic X(25) Using Relative-Data.
000382         05  Line 20 Column 01 Pic X(50) From  Error-Message.
000386*
000388 Procedure Division.
000389 Declaratives.
000390 Relative-File-Error Section.
000391     Use After Standard Error Procedure On Relative-File
000392     .
000393 Relative-Error.
000394     String "Error on Relative.Dat "
000395           Relative-Status
000396           Delimited By Size
000397           Into Error-Message
000398     End-String
000399     .
000400 End Declaratives.
000401 Chapt16a-Start.
000402     Perform Open-File
000418     If Not Relative-Success
000428        String "Error Opening Relative File "
000448               Relative-Status
000449               Delimited By Size
000450               Into Error-Message
000451        End-String
000452        Move Spaces To Relative-Data
000453        Perform Display-And-Accept
000454     Else
000455        Move Spaces To Relative-Data
000456        Perform Process-File Until F8-Pressed
000457        Perform Close-File
000458     End-If
000459     Stop Run
000460     .
000461 Process-File.
000462     Perform Display-And-Accept
000463     Evaluate True
000464       When F1-Pressed
000465            Perform Read-Random-Number
000466       When F2-Pressed
000467            Perform Start-Number
000468       When F3-Pressed
000469            Perform Read-Next-Number
000470       When F4-Pressed
000471            Perform Delete-Number
000472       When F5-Pressed
000473            Perform Write-Record
000474       When F6-Pressed
000475            Perform Rewrite-Record
000476       When F7-Pressed
000477            Perform Clear-Screen
000478       When F8-Pressed
000479            Continue
000480       When Other
000481            Continue
000482     End-Evaluate
000483     .
000484 Read-Random-Number.
000485     Read Relative-File
000486       Invalid Key
000487          String "Error on Random Read Number "
000488                 Relative-Status
000489                 Delimited By Size
000490                 Into Error-Message
000491     End-Read
000492     .
000501 Start-Number.
000502     Start Relative-File Key = Relative-Key
000503      Invalid Key
000504         String "Start Error Number "
000505                Relative-Status
000506                Delimited By Size
000507                Into Error-Message
000508      Not Invalid Key
000509         String "Start Successful "
000510                Relative-Status
000511                Delimited By Size
000512                Into Error-Message
000513     End-Start
000514     .
000521 Read-Next-Number.
000530     Read Relative-File Next
000531          At End
000532             Move "End of File " To Error-Message
000533     End-Read
000534     .
000536 Delete-Number.
000537     Delete Relative-File
000538      Invalid Key
000539         String "Delete Error "
000540                Relative-Status
000541                Delimited By Size
000542                Into Error-Message
000543      Not Invalid Key
000544         Move "Record Deleted" To Error-Message
000545         Perform Clear-Screen
000546     End-Delete
000547     .
000548 Write-Record.
000549     Write Relative-Record
000550       Invalid Key
000551          String "Write Error "
000552                 Relative-Status
000553                 Delimited By Size
000554                 Into Error-Message
000555       Not Invalid Key
000556          Move "Write Successful"
000557               To Error-Message
000558     End-Write
000560     .
000561 Rewrite-Record.
000562     Rewrite Relative-Record
000563         Invalid Key
000564            String "Rewrite Error "
000565                   Relative-Status
000566                   Delimited By Size
000567                   Into Error-Message
000568         Not Invalid Key
000569            Move "Rewrite Successful"
000570                 To Error-Message
000572     End-Rewrite
000573     .
000574 Clear-Screen.
000575     Initialize Relative-Record
000576     Move Zeros To Relative-Key
000577     Move 01 To Cursor-Row Cursor-Column
000578     .
000579 Display-And-Accept.
000580     Display Data-Entry-Screen
000581     Accept Data-Entry-Screen
000582     Move Spaces To Error-Message
000583     .
000584 Open-File.
000585     Open I-O Relative-File
000586     If Relative-Status = "05"
000587        Perform Create-Base-File Varying Relative-Key
000588                                 From 1 By 1
000589                                 Until Relative-Key  > 10  Or
000590                                 Not Relative-Success
000591        Perform Clear-Screen
000592     End-If
000593     .
000594 Create-Base-File.
000595     Write Relative-Record From Basic-Table (Relative-Key)
000596        Invalid Key
000597          String "Creation Write Error "
000598                 Relative-Status
000599                 Delimited By Size
000600                 Into Error-Message
000601          Perform Display-And-Accept
000602     End-Write
000603     .
000604 Close-File.
000605     Close Relative-File
000606     .
