000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt18x.
000031* Chapter 18 Exercise Solution
000043 Environment Division.
000050 Configuration Section.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000061 Input-Output  Section.
000062 File-Control.
000063     Select Optional Trans-File Assign To "Trans.Seq"
000064         Organization Is Line Sequential.
000066     Select Optional Dealer-File Assign To "Dealer.Seq"
000067         Organization Is Line Sequential.
000068     Select Optional Dealer-Out Assign To "Dealer.Out"
000069         Organization Is Line Sequential.
000073 Data Division.
000074 File Section.
000075 Fd  Trans-File.
000076 01  Trans-Record.
000077     03  Transaction-Date   Pic  9(8).
000078     03  Transaction-Text.
000079         05  Transaction-Type   Pic  X(4).
000080         05  Transaction-Dealer Pic  X(8).
000081     03  Transaction-Price  Pic S9(7)v99.
000082     03  Transaction-Qty    Pic  9(3).
000083     03  Filler             Pic  X(40).
000093 Fd  Dealer-File.
000094 01  Dealer-Record.
000095     03  Dealer-Number         Pic X(8).
000096     03  Dealer-Name.
000097         05  Last-Name   Pic X(25).
000098         05  First-Name  Pic X(15).
000099         05  Middle-Name Pic X(10).
000100     03  Address-Line-1      Pic X(50).
000101     03  Address-Line-2      Pic X(50).
000102     03  City                Pic X(40).
000103     03  State-Or-Country    Pic X(20).
000104     03  Postal-Code         Pic X(15).
000105     03  Home-Phone          Pic X(20).
000106     03  Work-Phone          Pic X(20).
000107     03  Other-Phone         Pic X(20).
000108     03  Start-Date          Pic 9(8).
000109     03  Last-Rent-Paid-Date Pic 9(8).
000110     03  Next-Rent-Due-Date  Pic 9(8).
000111     03  Rent-Amount         Pic 9(4)v99.
000112     03  Consignment-Percent Pic 9(3).
000113     03  Last-Sold-Amount    Pic S9(7)v99.
000114     03  Last-Sold-Date      Pic 9(8).
000115     03  Sold-To-Date        Pic S9(7)v99.
000116     03  Commission-To-Date  Pic S9(7)v99.
000117     03  Filler              Pic X(15).
000118 Fd  Dealer-Out.
000119 01  Dealer-Out-Record       Pic X(376).
000144 Working-Storage Section.
000145 01  Current-Commission      Pic S9(7)v99 Value Zeros.
000146 01  Total-Commission        Pic S9(7)v99 Value Zeros.
000154 01  Transactions-Read       Pic 9(5) Value Zeros.
000165 01  Master-Records-Written  Pic 9(5) Value Zeros.
000174 01  Work-Date.
000184     03  Work-MM             Pic 9(2).
000194     03  Work-DD             Pic 9(2).
000195     03  Work-YYYY           Pic 9(4).
000196 01  Reverse-Date.
000197     03  Work-YYYY           Pic 9(4).
000198     03  Work-MM             Pic 9(2).
000199     03  Work-DD             Pic 9(2).
000200 01  Compare-Date-1          Pic 9(8).
000201 01  Compare-Date-2          Pic 9(8).
000204 01  Edit-Count              Pic ZZ,ZZ9.
000205 01  Edit-Amt                Pic Z,ZZZ,ZZZ.99-.
000206* Serves As A Hold Area
000207 01  Hold-Record             Pic X(376).
000208* This Way We Know We Have A Master Record Held
000209 01  Create-Flag             Pic X Value Spaces.
000210     88  Creating-New-Master Value "Y".
000216 Procedure Division.
000232 Chapt18x-Start.
000233     Display "Begin Process Chapt18x"
000234     Open Output Dealer-Out
000236          Input  Trans-File
000245                 Dealer-File
000246     Perform Read-Dealer
000247     Perform Read-Trans
000255     Perform Process-Files Until
000265             Trans-Record = High-Values And
000275             Dealer-Record = High-Values
000285     Close Dealer-Out
000305           Trans-File
000315           Dealer-File
000316     Move Transactions-Read To Edit-Count
000317     Display "Processing Complete"
000318     Display "Transactions Read " Edit-Count
000319     Move Master-Records-Written To Edit-Count
000320     Display "Master Records Written " Edit-Count
000323     Move Total-Commission To Edit-Amt
000324     Display "Total Commission  " Edit-Amt
000325     Stop Run
000326     .
000327
000335 Process-Files.
000345     Evaluate True
000355       When Dealer-Number < Transaction-Dealer
000365            Perform Write-Dealer-Out
000375            Perform Read-Dealer
000385       When Dealer-Number > Transaction-Dealer
000395            Perform Create-New-Master
000425            Perform Read-Trans
000435       When Dealer-Number = Transaction-Dealer
000445            Perform Apply-Transaction
000465            Perform Read-Trans
000475     End-Evaluate
000485     .
000495 Apply-Transaction.
000505     Compute Sold-To-Date = Sold-To-Date +
000525             (Transaction-Qty * Transaction-Price)
000526     Compute Current-Commission Rounded =
000527             (Transaction-Qty * Transaction-Price) *
000528             (Consignment-Percent / 100)
000529     Add Current-Commission To Commission-To-Date
000530                               Total-Commission
000531     Move Last-Sold-Date To Work-Date
000532     Move Corresponding Work-Date To Reverse-Date
000533     Move Reverse-Date To Compare-Date-1
000534     Move Transaction-Date To Work-Date
000535     Move Corresponding Work-Date To Reverse-Date
000536     Move Reverse-Date To Compare-Date-2
000537     If Compare-Date-2 > Compare-Date-1
000538        Move Transaction-Date To
000539             Last-Sold-Date
000540     End-If
000541     .
000542 Write-Dealer-Out.
000543     Add 1 To Master-Records-Written
000552     Write Dealer-Out-Record From Dealer-Record
000562     .
000563 Create-New-Master.
000564     Set Creating-New-Master To True
000565* Move The Current Dealer Master To A Hold Area
000566     Move Dealer-Record To Hold-Record
000567* Fill In The Few Basic Fields We Know
000568     Initialize Dealer-Record
000569     Move Transaction-Dealer To Dealer-Number
000570                                Last-Name
000571* 10 Percent Default Consignment Percent
000572     Move 10 To Consignment-Percent
000573* Apply The Transaction Like Any Other
000574     Perform Apply-Transaction
000575     .
000576 Read-Dealer.
000577* Before Reading, See If A Record Is "held", If So, Move It In
000578* Instead Of Reading A New Record.
000579* Then Reset The Flag, Because One Is No Longer Held.
000580     If Creating-New-Master
000581        Move Hold-Record To Dealer-Record
000582        Move Space To Create-Flag
000583     Else
000584        Read Dealer-File
000592             At End
000602             Move High-Values To Dealer-Record
000612        End-Read
000613     End-If
000622     .
000632 Read-Trans.
000642     Read Trans-File
000652          At End
000662             Move High-Values To Trans-Record
000663          Not At End
000664             Add 1 To Transactions-Read
000672     End-Read
000682     .
