000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt20x.
000031* Chapter 20 Exercise Answer
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
000153     03  Sort-Trans-Price        Pic S9(6)v99.
000154     03  Sort-Trans-Qty          Pic 9(3).
000155     03  Sort-Commission         Pic S9(6)v99.
000156     03  Sort-Dealer-Number      Pic X(8).
000157 Working-Storage Section.
000209 01  Heading-Line-1.
000210     03  Filler      Pic X(12) Value "Created by:".
000211     03  Filler      Pic X(8)  Value "CHAPT20X".
000212     03  Filler      Pic X(8) Value Spaces.
000213     03  Filler      Pic X(29)
000214         Value "Transaction Detail by Dealer".
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
000232* Heading Line Must Be Modified.
000235 01  Heading-Line-4.
000236     03  Filler      Pic X(10) Value "Dealer".
000237     03  Filler      Pic X(28) Value "Name".
000238     03  Filler      Pic X(8)  Value "Date   ".
000239     03  Filler      Pic X(7)  Value "Type ".
000240     03  Filler      Pic X(8)  Value "Qty".
000241     03  Filler      Pic X(8)  Value "Amount".
000242     03  Filler      Pic X(10) Value "Commission".
000243 01  Blank-Line      Pic X(80) Value Spaces.
000244* Detail Line Is New.
000253 01  Detail-Line.
000254     03  Detail-Dealer       Pic X(8)  Value Spaces.
000255     03  Filler              Pic X     Value Spaces.
000256     03  Detail-Dealer-Name  Pic X(25) Value Spaces.
000257     03  Filler              Pic X     Value Spaces.
000258     03  Detail-Date.
000259         05  Trans-Month     Pic 99.
000260         05  Filler          Pic X Value "/".
000261         05  Trans-Day       Pic 99.
000262         05  Filler          Pic X Value "/".
000263         05  Trans-Year      Pic 9(4).
000264     03  Filler              Pic X     Value Spaces.
000265     03  Detail-Type         Pic X(4)  Value Spaces.
000266     03  Filler              Pic X     Value Spaces.
000267     03  Detail-Qty          Pic Z(4)9.
000268     03  Filler              Pic X     Value Spaces.
000269     03  Detail-Amt          Pic $$$,$$$.99-.
000270     03  Filler              Pic X     Value Spaces.
000271     03  Detail-Commission   Pic $$$,$$$.99-.
000272 01  Total-Line.
000273     03  Total-Description   Pic X(51)       Value Spaces.
000274     03  Total-Qty           Pic Z(4)9.
000275     03  Filler              Pic X           Value Spaces.
000276     03  Total-Amt           Pic $$$,$$$.99-.
000277     03  Filler              Pic X           Value Spaces.
000278     03  Total-Commission    Pic $$$,$$$.99-.
000279 01  Desc-Type.
000280     03  Filler              Pic X(11) Value "*   Total".
000281     03  Desc-Type-Type      Pic X(4).
000282 01  Desc-Date.
000283     03  Filler              Pic X(11) Value "**  Total".
000284     03  Trans-Month         Pic 99.
000285     03  Filler              Pic X Value "/".
000286     03  Trans-Day           Pic 99.
000287     03  Filler              Pic X Value "/".
000288     03  Trans-Year          Pic 9(4).
000289 01  Desc-Dealer.
000290     03  Filler              Pic X(11) Value "*** Total".
000291     03  Desc-Dealer-Name    Pic X(30).
000292 01  Save-Fields.
000293     03  Save-Dealer-Name                Value High-Values.
000294         05  Last-Name      Pic X(25).
000295         05  First-Name     Pic X(15).
000296         05  Middle-Name    Pic X(10).
000297     03  Save-Date-X.
000298         05  Trans-Year     Pic 9(4).
000299         05  Trans-Month    Pic 9(2).
000300         05  Trans-Day      Pic 9(2).
000301     03 Save-Type           Pic X(4)     Value High-Values.
000302 01  Accumulators.
000303     03  Grand-Totals.
000304         05  Total-Qty        Pic 9(5)         Value Zeros.
000305         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000306         05  Total-Commission Pic S9(5)v99     Value Zeros.
000307     03  Dealer-Totals.
000308         05  Total-Qty        Pic 9(5)         Value Zeros.
000309         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000310         05  Total-Commission Pic S9(5)v99     Value Zeros.
000311     03  Date-Totals.
000312         05  Total-Qty        Pic 9(5)         Value Zeros.
000313         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000314         05  Total-Commission Pic S9(5)v99     Value Zeros.
000315     03  Type-Totals.
000316         05  Total-Qty        Pic 9(5)         Value Zeros.
000317         05  Total-Amt        Pic S9(6)v99     Value Zeros.
000318         05  Total-Commission Pic S9(5)v99     Value Zeros.
000319 01  Line-Count           Pic 99          Value 99.
000320 01  Page-Count           Pic 9(4)        Value Zeros.
000321 01  Max-Lines            Pic 99          Value 60.
000322 01  Date-And-Time-Area.
000323     03  Work-Date            Pic 9(6).
000324     03  Work-Date-X          Redefines Work-Date.
000325         05  Date-YY          Pic 99.
000326         05  Date-MM          Pic 99.
000327         05  Date-DD          Pic 99.
000328     03  Work-Time            Pic 9(8).
000329     03  Work-Time-X          Redefines Work-Time.
000330         05  Time-HH          Pic 99.
000331         05  Time-MM          Pic 99.
000332         05  Time-SS          Pic 99.
000333         05  Filler           Pic XX.
000334 01  String-Pointer       Pic 99 Value Zeros.
000335 01  Done-Flag            Pic X Value Spaces.
000336     88  All-Done               Value "Y".
000337 01  Dealer-Status           Pic XX Value Zeros.
000338     88  Dealer-Success  Value "00" Thru "09".
000339 Procedure Division.
000340 Declaratives.
000341 Dealer-File-Error Section.
000342     Use After Standard Error Procedure On Dealer-File
000343     .
000344 Dealer-Error-Paragraph.
000345     Display "Error on Dealer File " Dealer-Status
000346     .
000347 End Declaratives.
000348 Chapt20x-Start.
000349     Display "Begin Process Chapt20X"
000350     Sort Sort-File Ascending Key Sort-Key
000351          Input Procedure Sort-In
000352          Output Procedure Print-Report
000353     Stop Run
000354     .
000355 Sort-In.
000356     Open Input Trans-File
000357                Dealer-File
000358     Perform Process-Input-Records Until All-Done
000359     Close Trans-File
000360           Dealer-File
000361     .
000362 Process-Input-Records.
000363     Read Trans-File
000364        At End Set All-Done To True
000365        Not At End
000366            Perform Move-And-Release-Input
000367     End-Read
000368     .
000369 Move-And-Release-Input.
000370* Reverse The Date
000371     Move Corresponding Transaction-Date-X To
000372                        Sort-Trans-Date
000373* Move The Data
000374     Move Transaction-Price  To Sort-Trans-Price
000375     Move Transaction-Qty    To Sort-Trans-Qty
000376     Move Transaction-Type   To Sort-Trans-Type
000377     Move Transaction-Dealer To Sort-Dealer-Number
000378* Read Dealer File To Retrieve Name And Consignment Percent
000379     Perform Retrieve-Dealer-Record
000380* Move The Name And Compute Consignment
000381     Move Dealer-Name Of Dealer-Record To
000382          Dealer-Name Of Sort-Record
000383     Compute Sort-Commission Rounded =
000384             (Transaction-Qty * Transaction-Price) *
000385             (Consignment-Percent / 100)
000386* Release The Record
000387     Release Sort-Record
000388     .
000389 Retrieve-Dealer-Record.
000390     Move Transaction-Dealer To Dealer-Number Of Dealer-Record
000391     Read Dealer-File
000392          Invalid Key
000393             Move "**UNKNOWN**" To
000394                  Dealer-Name Of Dealer-Record
000395             Move 10 To Consignment-Percent
000396     End-Read
000397     .
000398 Print-Report.
000399     Open Output Report-File
000400     Move Space To Done-Flag
000401     Perform Fill-Initial-Headings
000402     Perform Return-Process-Records Until All-Done
000403     Close Report-File
000404     .
000405 Return-Process-Records.
000406     Return Sort-File
000407            At End
000408               Perform Type-Break
000409               Perform Date-Break
000410               Perform Dealer-Break
000411               Perform Print-Grand-Totals
000412               Set All-Done To True
000413            Not At End
000414               Perform Check-For-Break
000415     End-Return
000416     .
000417 Check-For-Break.
000418     Evaluate True
000419        When  Save-Dealer-Name = High-Values
000420              Move Sort-Key To Save-Fields
000421        When  Dealer-Name Of Sort-Record Not = Save-Dealer-Name
000422              Perform Type-Break
000423              Perform Date-Break
000424              Perform Dealer-Break
000425        When  Sort-Trans-Date Not = Save-Date-X
000426              Perform Type-Break
000427              Perform Date-Break
000428        When  Sort-Trans-Type Not = Save-Type
000429              Perform Type-Break
000430        When  Other
000431              Continue
000432     End-Evaluate
000433     Perform Accumulate-Details
000434     .
000435 Accumulate-Details.
000436* New Detail Record Logic
000437     Perform Fill-Write-Detail
000438     Add Sort-Trans-Qty To Total-Qty Of Type-Totals
000439     Add Sort-Commission To Total-Commission Of Type-Totals
000440     Compute Total-Amt Of Type-Totals =
000441             Total-Amt Of Type-Totals +
000442             (Sort-Trans-Qty * Sort-Trans-Price)
000443     .
000444 Fill-Write-Detail.
000445* Notice The Qualification Of First-Name, Middle-Name
000446* And Last-Name Is All The Way Up To The Record Level And
000447* Not The Group Name The Immediately Precedes Them In
000448* The Sort Record?  This Is Because That Group Is Also
000449* Duplicated.  When Qualifying Data Fields, You Should Try
000450* To Use The Lowest Level That Provides A Unique Qualification.
000451     Move Sort-Dealer-Number To Detail-Dealer
000452     Move Spaces To Detail-Dealer-Name
000453     Move 1 To String-Pointer
000454     String First-Name Of Sort-Record
000455                         Delimited By Space
000456            Into Detail-Dealer-Name
000457            With Pointer String-Pointer
000458     End-String
000459     If Middle-Name Of Sort-Record
000460        > Spaces
000461        String " " Delimited By Size
000462               Middle-Name Of Sort-Record
000463                   Delimited By Spaces
000464               Into Detail-Dealer-Name
000465               With Pointer String-Pointer
000466        End-String
000467     End-If
000468     String " " Delimited By Size
000469            Last-Name Of Sort-Record
000470                   Delimited By Spaces
000471               Into Detail-Dealer-Name
000472               With Pointer String-Pointer
000473     End-String
000474     Move Corresponding Sort-Trans-Date To Detail-Date
000475     Move Sort-Trans-Type To Detail-Type
000476     Move Sort-Trans-Qty  To Detail-Qty
000477     Compute Detail-Amt = Sort-Trans-Qty * Sort-Trans-Price
000478     Move Sort-Commission To Detail-Commission
000479     If Line-Count > Max-Lines
000480        Perform Heading-Routine
000481     End-If
000482     Write Report-Record From Detail-Line After 1
000483     .
000484 Type-Break.
000485     Perform Print-Type-Total
000486     Add Corresponding Type-Totals To Date-Totals
000487     Initialize Type-Totals
000488     Move Sort-Trans-Type To Save-Type
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
000549* Changed This Paragraph To Double Space.
000558     Move Corresponding Type-Totals To Total-Line
000568     Move Save-Type To Desc-Type-Type
000578     Move Desc-Type To Total-Description
000579     If Line-Count > Max-Lines - 2
000580        Perform Heading-Routine
000581     End-If
000582     Write Report-Record From Total-Line After 2
000583     Write Report-Record From Blank-Line After 1
000592     Add 3 To Line-Count
000598     .
000608 Print-Date-Total.
000618     Move Corresponding Date-Totals To Total-Line
000628     Move Corresponding Save-Date-X To Desc-Date
000638     Move Desc-Date To Total-Description
000639     If Line-Count > Max-Lines - 1
000640        Perform Heading-Routine
000641     End-If
000648     Write Report-Record From Total-Line After 1
000649     Write Report-Record From Blank-Line After 1
000658     Add 2 To Line-Count
000659     .
000668 Print-Dealer-Total.
000678     Move Corresponding Dealer-Totals To Total-Line
000715     Move Spaces To Desc-Dealer-Name
000716     Move 1 To String-Pointer
000717     String First-Name Of Save-Dealer-Name
000718                         Delimited By Space
000719            Into Desc-Dealer-Name
000720            With Pointer String-Pointer
000721     End-String
000722     If Middle-Name Of Save-Dealer-Name
000723        > Spaces
000724        String " " Delimited By Size
000725               Middle-Name Of Save-Dealer-Name
000726                   Delimited By Spaces
000727               Into Desc-Dealer-Name
000728               With Pointer String-Pointer
000729        End-String
000730     End-If
000731     String " " Delimited By Size
000732            Last-Name Of Save-Dealer-Name
000733                   Delimited By Spaces
000734               Into Desc-Dealer-Name
000735               With Pointer String-Pointer
000736     End-String
000737     Move Desc-Dealer To Total-Description
000738     If Line-Count > Max-Lines - 1
000739        Perform Heading-Routine
000740     End-If
000741     Write Report-Record From Total-Line After 1
000742     Write Report-Record From Blank-Line After 1
000743     Add 2 To Line-Count
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
