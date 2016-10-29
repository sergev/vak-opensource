000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt20a.
000031* Control Breaks
000043 Environment Division.
000050 Configuration Section.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000061 Input-Output  Section.
000062 File-Control.
000063     Select Dealer-File Assign To "Dealer.Dat"
000064         Organization Indexed
000065         Access Random
000066         Record Key Dealer-Number
000067         Alternate Record Key Dealer-Name Of Dealer-Record
000068         File Status Dealer-Status.
000070     Select Report-File Assign To Printer.
000071     Select Optional Trans-File Assign To "Trans1.TXT"
000072            Organization Is Line Sequential.
000073     Select Sort-File Assign To Sort-Work.
000076 Data Division.
000077 File Section.
000085 Fd  Dealer-File.
000086 01  Dealer-Record.
000087     03  Dealer-Number         Pic X(8).
000088     03  Dealer-Name.
000089         05  Last-Name   Pic X(25).
000090         05  First-Name  Pic X(15).
000091         05  Middle-Name Pic X(10).
000092     03  Address-Line-1      Pic X(50).
000093     03  Address-Line-2      Pic X(50).
000094     03  City                Pic X(40).
000095     03  State-Or-Country    Pic X(20).
000096     03  Postal-Code         Pic X(15).
000097     03  Home-Phone          Pic X(20).
000098     03  Work-Phone          Pic X(20).
000099     03  Other-Phone         Pic X(20).
000100     03  Start-Date          Pic 9(8).
000101     03  Last-Rent-Paid-Date Pic 9(8).
000102     03  Next-Rent-Due-Date  Pic 9(8).
000103     03  Rent-Amount         Pic 9(4)v99.
000104     03  Consignment-Percent Pic 9(3).
000105     03  Last-Sold-Amount    Pic S9(7)v99.
000106     03  Last-Sold-Date      Pic 9(8).
000107     03  Sold-To-Date        Pic S9(7)v99.
000108     03  Commission-To-Date  Pic S9(7)v99.
000109     03  Filler              Pic X(15).
000110 Fd  Report-File.
000111 01  Report-Record Pic X(80).
000112 Fd  Trans-File.
000113 01  Trans-Record.
000114     03  Transaction-Date   Pic  9(8).
000115     03  Transaction-Date-X Redefines Transaction-Date.
000116         05  Trans-Month    Pic 99.
000117         05  Trans-Day      Pic 99.
000118         05  Trans-Year     Pic 9(4).
000119     03  Transaction-Type   Pic  X(4).
000120     03  Transaction-Dealer Pic  X(8).
000121     03  Transaction-Price  Pic S9(7)v99.
000122     03  Transaction-Qty    Pic  9(3).
000123     03  Filler             Pic  X(40).
000124 Sd  Sort-File.
000137 01  Sort-Record.
000138     03  Sort-Key.
000141         05  Dealer-Name.
000142             10  Last-Name               Pic X(25).
000143             10  First-Name              Pic X(15).
000144             10  Middle-Name             Pic X(10).
000145         05  Sort-Trans-Date.
000147             10  Trans-Year              Pic 9(4).
000149             10  Trans-Month             Pic 9(2).
000150             10  Trans-Day               Pic 9(2).
000151         05  Sort-Trans-Type             Pic X(4).
000152     03  Sort-Trans-Price        Pic S9(6)v99.
000153     03  Sort-Trans-Qty          Pic 9(3).
000154     03  Sort-Commission         Pic S9(6)v99.
000156 Working-Storage Section.
000209 01  Heading-Line-1.
000210     03  Filler      Pic X(12) Value "Created by:".
000211     03  Filler      Pic X(8)  Value "CHAPT20A".
000212     03  Filler      Pic X(8) Value Spaces.
000213     03  Filler      Pic X(29)
000214         Value "Transaction Summary by Dealer".
000215     03  Filler      Pic X(7) Value Spaces.
000216     03  Filler      Pic X(5)  Value "Page".
000217     03  Page-No     Pic Z(4)9 Value Zeros.
000218 01  Heading-Line-2.
000219     03  Filler      Pic X(12) Value "Created on:".
000220     03  Date-MM     Pic 99.
000221     03  Filler      Pic X     Value "/".
000222     03  Date-DD     Pic 99.
000223     03  Filler      Pic X     Value "/".
000224     03  Date-YY     Pic 99.
000225 01  Heading-Line-3.
000226     03  Filler      Pic X(12) Value "At:".
000227     03  Time-HH     Pic 99.
000228     03  Filler      Pic X     Value ":".
000229     03  Time-MM     Pic 99.
000230     03  Filler      Pic X     Value ":".
000231     03  Time-SS     Pic 99.
000235 01  Heading-Line-4.
000236     03  Filler      Pic X(51) Value Spaces.
000239     03  Filler      Pic X(6)  Value "  Qty".
000240     03  Filler      Pic X(12) Value "    Amount".
000241     03  Filler      Pic X(10) Value "Commission".
000242 01  Blank-Line      Pic X(80) Value Spaces.
000261 01  Total-Line.
000262     03  Total-Description   Pic X(51)       Value Spaces.
000263     03  Total-Qty           Pic Z(4)9.
000264     03  Filler              Pic X           Value Spaces.
000265     03  Total-Amt           Pic $$$,$$$.99-.
000266     03  Filler              Pic X           Value Spaces.
000267     03  Total-Commission    Pic $$$,$$$.99-.
000268 01  Desc-Type.
000269     03  Filler              Pic X(11) Value "*   Total".
000270     03  Desc-Type-Type      Pic X(4).
000271 01  Desc-Date.
000272     03  Filler              Pic X(11) Value "**  Total".
000273     03  Trans-Month         Pic 99.
000274     03  Filler              Pic X Value "/".
000275     03  Trans-Day           Pic 99.
000276     03  Filler              Pic X Value "/".
000277     03  Trans-Year          Pic 9(4).
000278 01  Desc-Dealer.
000279     03  Filler              Pic X(11) Value "*** Total".
000280     03  Desc-Dealer-Name    Pic X(30).
000281 01  Save-Fields.
000282     03  Save-Dealer-Name                Value High-Values.
000283         05  Last-Name      Pic X(25).
000284         05  First-Name     Pic X(15).
000285         05  Middle-Name    Pic X(10).
000286     03  Save-Date-X.
000287         05  Trans-Year     Pic 9(4).
000288         05  Trans-Month    Pic 9(2).
000289         05  Trans-Day      Pic 9(2).
000290     03 Save-Type           Pic X(4)     Value High-Values.
000291 01  Accumulators.
000292     03  Grand-Totals.
000293         05  Total-Qty        Pic 9(5)         Value Zeros.
000294         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000295         05  Total-Commission Pic S9(5)v99     Value Zeros.
000298     03  Dealer-Totals.
000299         05  Total-Qty        Pic 9(5)         Value Zeros.
000300         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000301         05  Total-Commission Pic S9(5)v99     Value Zeros.
000302     03  Date-Totals.
000303         05  Total-Qty        Pic 9(5)         Value Zeros.
000304         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000305         05  Total-Commission Pic S9(5)v99     Value Zeros.
000306     03  Type-Totals.
000307         05  Total-Qty        Pic 9(5)         Value Zeros.
000308         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000309         05  Total-Commission Pic S9(5)v99     Value Zeros.
000310 01  Line-Count           Pic 99          Value 99.
000311 01  Page-Count           Pic 9(4)        Value Zeros.
000312 01  Max-Lines            Pic 99          Value 60.
000318 01  Date-And-Time-Area.
000319     03  Work-Date            Pic 9(6).
000320     03  Work-Date-X          Redefines Work-Date.
000321         05  Date-YY          Pic 99.
000322         05  Date-MM          Pic 99.
000323         05  Date-DD          Pic 99.
000324     03  Work-Time            Pic 9(8).
000325     03  Work-Time-X          Redefines Work-Time.
000326         05  Time-HH          Pic 99.
000327         05  Time-MM          Pic 99.
000328         05  Time-SS          Pic 99.
000329         05  Filler           Pic XX.
000330 01  String-Pointer       Pic 99 Value Zeros.
000331 01  Done-Flag            Pic X Value Spaces.
000332     88  All-Done               Value "Y".
000333 01  Dealer-Status           Pic XX Value Zeros.
000334     88  Dealer-Success  Value "00" Thru "09".
000335 Procedure Division.
000336 Declaratives.
000337 Dealer-File-Error Section.
000338     Use After Standard Error Procedure On Dealer-File
000339     .
000340 Dealer-Error-Paragraph.
000341     Display "Error on Dealer File " Dealer-Status
000342     .
000343 End Declaratives.
000344 Chapt20a-Start.
000345     Display "Begin Process Chapt20A"
000346     Sort Sort-File Ascending Key Sort-Key
000347          Input Procedure Sort-In
000348          Output Procedure Print-Report
000349     Stop Run
000350     .
000351 Sort-In.
000352     Open Input Trans-File
000353                Dealer-File
000354     Perform Process-Input-Records Until All-Done
000355     Close Trans-File
000356           Dealer-File
000357     .
000358 Process-Input-Records.
000359     Read Trans-File
000360        At End Set All-Done To True
000361        Not At End
000362            Perform Move-And-Release-Input
000363     End-Read
000364     .
000365 Move-And-Release-Input.
000366* Reverse The Date
000367     Move Corresponding Transaction-Date-X To
000368                        Sort-Trans-Date
000369* Move The Data
000370     Move Transaction-Price  To Sort-Trans-Price
000371     Move Transaction-Qty    To Sort-Trans-Qty
000372     Move Transaction-Type   To Sort-Trans-Type
000373* Read Dealer File To Retrieve Name And Consignment Percent
000374     Perform Retrieve-Dealer-Record
000375* Move The Name And Compute Consignment
000376     Move Dealer-Name Of Dealer-Record To
000377          Dealer-Name Of Sort-Record
000378     Compute Sort-Commission Rounded =
000379             (Transaction-Qty * Transaction-Price) *
000380             (Consignment-Percent / 100)
000381* Release The Record
000382     Release Sort-Record
000383     .
000384 Retrieve-Dealer-Record.
000385     Move Transaction-Dealer To Dealer-Number Of Dealer-Record
000386     Read Dealer-File
000387          Invalid Key
000388             Move "**UNKNOWN**" To
000389                  Dealer-Name Of Dealer-Record
000390             Move 10 To Consignment-Percent
000391     End-Read
000392     .
000393 Print-Report.
000394     Open Output Report-File
000395     Move Space To Done-Flag
000396     Perform Fill-Initial-Headings
000397     Perform Return-Process-Records Until All-Done
000398     Close Report-File
000399     .
000400 Return-Process-Records.
000401     Return Sort-File
000402            At End
000403               Perform Type-Break
000404               Perform Date-Break
000405               Perform Dealer-Break
000406               Perform Print-Grand-Totals
000407               Set All-Done To True
000408            Not At End
000409               Perform Check-For-Break
000410     End-Return
000411     .
000412 Check-For-Break.
000413     Evaluate True
000414        When  Save-Dealer-Name = High-Values
000415              Move Sort-Key To Save-Fields
000416        When  Dealer-Name Of Sort-Record Not = Save-Dealer-Name
000417              Perform Type-Break
000418              Perform Date-Break
000419              Perform Dealer-Break
000420        When  Sort-Trans-Date Not = Save-Date-X
000421              Perform Type-Break
000422              Perform Date-Break
000423        When  Sort-Trans-Type Not = Save-Type
000424              Perform Type-Break
000425        When  Other
000426              Continue
000427     End-Evaluate
000428     Perform Accumulate-Details
000429     .
000430 Accumulate-Details.
000431     Add Sort-Trans-Qty To Total-Qty Of Type-Totals
000432     Add Sort-Commission To Total-Commission Of Type-Totals
000433     Compute Total-Amt Of Type-Totals =
000434             Total-Amt Of Type-Totals +
000435             (Sort-Trans-Qty * Sort-Trans-Price)
000436     .
000437 Type-Break.
000457     Perform Print-Type-Total
000458     Add Corresponding Type-Totals To Date-Totals
000467     Initialize Type-Totals
000468     Move Sort-Trans-Type To Save-Type
000517     .
000527 Date-Break.
000537     Perform Print-Date-Total
000538     Add Corresponding Date-Totals To Dealer-Totals
000539     Initialize Date-Totals
000540     Move Sort-Trans-Date To Save-Date-X
000541     .
000542 Dealer-Break.
000543     Perform Print-Dealer-Total
000544     Add Corresponding Dealer-Totals To Grand-Totals
000545     Initialize Dealer-Totals
000546     Move Dealer-Name Of Sort-Record To Save-Dealer-Name
000547     .
000548 Print-Type-Total.
000558     Move Corresponding Type-Totals To Total-Line
000568     Move Save-Type To Desc-Type-Type
000578     Move Desc-Type To Total-Description
000579     If Line-Count > Max-Lines
000580        Perform Heading-Routine
000581     End-If
000582     Write Report-Record From Total-Line After 1
000592     Add 1 To Line-Count
000598     .
000608 Print-Date-Total.
000618     Move Corresponding Date-Totals To Total-Line
000628     Move Corresponding Save-Date-X To Desc-Date
000638     Move Desc-Date To Total-Description
000639     If Line-Count > Max-Lines - 2
000640        Perform Heading-Routine
000641     End-If
000648     Write Report-Record From Total-Line After 2
000649     Write Report-Record From Blank-Line After 1
000658     Add 3 To Line-Count
000659     .
000668 Print-Dealer-Total.
000678     Move Corresponding Dealer-Totals To Total-Line
000679     Move Spaces To Desc-Dealer-Name
000680     Move 1 To String-Pointer
000686     String First-Name Of Save-Dealer-Name
000688                         Delimited By Space
000689            Into Desc-Dealer-Name
000690            With Pointer String-Pointer
000691     End-String
000692     If Middle-Name Of Save-Dealer-Name
000693        > Spaces
000694        String " " Delimited By Size
000695               Middle-Name Of Save-Dealer-Name
000696                   Delimited By Spaces
000697               Into Desc-Dealer-Name
000698               With Pointer String-Pointer
000699        End-String
000700     End-If
000701     String " " Delimited By Size
000702            Last-Name Of Save-Dealer-Name
000703                   Delimited By Spaces
000704               Into Desc-Dealer-Name
000705               With Pointer String-Pointer
000706     End-String
000715     Move Desc-Dealer To Total-Description
000716     If Line-Count > Max-Lines - 1
000717        Perform Heading-Routine
000718     End-If
000728     Write Report-Record From Total-Line After 1
000729     Write Report-Record From Blank-Line After 1
000738     Add 2 To Line-Count
000748     .
000758 Print-Grand-Totals.
000768     Move Corresponding Grand-Totals To Total-Line
000778     Move "****Grand Totals" To Total-Description
000788     If Line-Count > Max-Lines - 1
000798        Perform Heading-Routine
000808     End-If
000818     Write Report-Record From Total-Line After 2
000828     .
000838 Heading-Routine.
000848     Add 1 To Page-Count
000849     Move Page-Count To Page-No
000858     If Page-Count = 1
000868        Write Report-Record From Heading-Line-1 After Zero
000878     Else
000888        Write Report-Record From Heading-Line-1 After Page
000898     End-If
000908     Write Report-Record From Heading-Line-2 After 1
000918     Write Report-Record From Heading-Line-3 After 1
000919     Write Report-Record From Heading-Line-4 After 2
000920     Write Report-Record From Blank-Line     After 1
000921     Move 6 To Line-Count
000922     .
000932 Fill-Initial-Headings.
000942     Accept Work-Date From Date
000952     Accept Work-Time From Time
000962     Move Corresponding Work-Date-X To
000972                        Heading-Line-2
000982     Move Corresponding Work-Time-X To
000992                        Heading-Line-3
001002     .