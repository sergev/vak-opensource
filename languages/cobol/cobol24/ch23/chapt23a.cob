000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt23a.
000040 Environment Division.
000050 Configuration Section.
000060 Source-Computer.  IBM-PC.
000070 Object-Computer.  IBM-PC.
000071 Special-Names.
000073     Crt Status Is Keyboard-Status.
000074 Data Division.
000090 Working-Storage Section.
000253 01  Dummy-Field         Pic X Value Spaces.
000254 01  Keyboard-Status.
000255     03  Accept-Status Pic 9.
000256     03  Function-Key  Pic X.
000257         88  F1-Pressed Value X"01".
000258         88  F2-Pressed Value X"02".
000259         88  F3-Pressed Value X"03".
000260     03  System-Use    Pic X.
000261 01  Done-Flag         Pic X Value Spaces.
000262     88  All-Done      Value "Y".
000263 Screen Section.
000264 01  Main-Screen
000265     Blank Screen, Auto, Required,
000266     Foreground-Color Is 7,
000267     Background-Color Is 1.
000268     03  Line 1 Column 29 Value "Program Selection Menu".
000269     03  Line 3 Column 1  Value "F1  Telephone Number Format".
000270     03  Line 5 Column 1  Value "F2  Days Between Dates".
000271     03  Line 7 Column 1  Value "F3  Exit".
000272     03  Line 9 Column 1  Pic X To Dummy-Field Secure.
000285 Procedure Division.
000286 Chapt023a-Start.
000287     Perform Until All-Done
000288       Display Main-Screen
000297       Accept Main-Screen
000298       Evaluate True
000307          When F1-Pressed
000317               Call "Chapt23b"
000318               Cancel "Chapt23b"
000327          When F2-Pressed
000337               Call "Chapt23c"
000338          When F3-Pressed
000339               Set All-Done To True
000347          When Other
000357               Continue
000367       End-Evaluate
000377     End-Perform
000460     Stop Run
000470     .