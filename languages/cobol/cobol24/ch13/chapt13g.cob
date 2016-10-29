000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt13g.
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
000068 01  Name-Record.
000069     03  Name-Ctr      Pic 9(2).
000070     03  Name-Table Occurs 1 To 20 Times Depending On Name-Ctr.
000071         05  Name-Item Pic X(20).