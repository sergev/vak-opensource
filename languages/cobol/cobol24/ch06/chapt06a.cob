000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt06a.
000031* Command Line Argument
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Special-Names.
000057     Argument-Value Is Command-Line.
000058 Data Division.
000059 Working-Storage Section.
000067 01  Command-Line-Argument Pic X(80).
000077 Procedure Division.
000159 Chapt06a-Start.
000167     Accept Command-Line-Argument From Command-Line
000169     Display "Command Line: " Command-Line-Argument
000187	   Stop Run
000198     .