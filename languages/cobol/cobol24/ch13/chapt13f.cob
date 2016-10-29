000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt13f.
000031* Update Example
000043 Environment Division.
000050 Configuration Section.
000051 Special-Names.
000052       Crt Status Is Keyboard-Status.
000054 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Input-Output  Section.
000059 File-Control.
000060     Select Optional Name-File Assign To "NAMES.SEQ"
000061         Organization Is Sequential
000062         File Status  Is Name-File-Status.
000065 Data Division.
000066 File Section.
000067 Fd  Name-File.
000068 01  Name-Record           Pic X(30).
000070 Working-Storage Section.
000071 01  Full-Name             Pic X(30) Value Spaces.
000073 01  Keyboard-Status.
000074     03  Accept-Status     Pic 9.
000075     03  Function-Key      Pic X.
000076         88 F1-Pressed     Value X"01".
000077         88 F2-Pressed     Value X"02".
000078     03  System-Use        Pic X.
000080 01  File-Error-Flag       Pic X Value Space.
000081     88  File-Error        Value "Y".
000082 01  Name-File-Status      Pic XX Value Spaces.
000083     88  Name-File-Success Value "00" "05".
000088     88  End-Of-File       Value "10".
000089 01  Error-Message         Pic X(50) Value Spaces.
000090 Screen Section.
000091 01  Name-Entry Blank Screen.
000092     03  Line 01 Column 01 Value " Enter Name: ".
000093     03  Line 01 Column 14 Pic X(30) Using Full-Name.
000094     03  Line 05 Column 01 Pic X(50) From Error-Message.
000095     03  Line 20 Column 01
000096         Value "Press F1 to Exit    Press F2 to Update".
000098 Procedure Division.
000159 Chapt13f-Start.
000160     Perform Open-File
000161     If Not File-Error
000162        Perform Process-File Until F1-Pressed Or
000163                                   File-Error Or
000164                                   End-Of-File
000165        Perform Close-File
000166     End-If
000167     Stop Run
000168     .
000177 Open-File.
000187     Open I-O Name-File
000197     If Not Name-File-Success
000207        Move Spaces To Error-Message
000217        String "Open Error " Name-File-Status
000227               Delimited By Size
000237               Into Error-Message
000247        Perform Display-And-Accept-Error
000257     End-If
000267     .
000277 Process-File.
000287     Move Spaces To Full-Name
000288     Perform Read-File
000289     If Not File-Error
000290        Display Name-Entry
000297        Accept Name-Entry
000298        Move Spaces To Error-Message
000299        If F2-Pressed And Not End-Of-File
000300           Perform Rewrite-Record
000301        End-If
000304     End-If
000317     .
000327 Read-File.
000337     Read Name-File Into Full-Name
000338          At End Move "End Of File" To Error-Message
000339     End-Read
000340     If Name-File-Success Or End-Of-File
000341        Continue
000342     Else
000343        Move Spaces To Error-Message
000344        String "Read Error " Name-File-Status
000345           Delimited By Size Into Error-Message
000346        End-String
000347        Perform Display-And-Accept-Error
000348     End-If
000349     .
000355 Rewrite-Record.
000365     Rewrite Name-Record From Full-Name
000366     If Name-File-Success
000367        Move "Prior Record Updated" To Error-Message
000368     Else
000375        Move Spaces To Error-Message
000395        String "Rewrite Error " Name-File-Status
000405          Delimited By Size Into Error-Message
000415        End-String
000417     End-If
000418     .
000427 Display-And-Accept-Error.
000428     Set File-Error To True
000437     Display Name-Entry
000447     Accept Name-Entry
000457     .
000467 Close-File.
000477     Close Name-File
000487     .