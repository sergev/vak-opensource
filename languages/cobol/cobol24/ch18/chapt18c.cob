000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt18c.
000031* Sequential File Update
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
000070     Select Optional Reject-File Assign To "Reject.Txt"
000071         Organization Is Line Sequential.
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
000084 Fd  Reject-File.
000085 01  Reject-Record          Pic X(72).
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
000164 01  Transactions-Rejected   Pic 9(5) Value Zeros.
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
000206 Procedure Division.
000207 Chapt18c-Start.
000208     Display "Begin Process Chapt18c"
000215     Open Output Reject-File
000225                 Dealer-Out
000235          Input  Trans-File
000245                 Dealer-File
000246     Perform Read-Dealer
000247     Perform Read-Trans
000255     Perform Process-Files Until
000265             Trans-Record = High-Values And
000275             Dealer-Record = High-Values
000285     Close Reject-File
000295           Dealer-Out
000305           Trans-File
000315           Dealer-File
000316     Move Transactions-Read To Edit-Count
000317     Display "Processing Complete"
000318     Display "Transactions Read " Edit-Count
000319     Move Transactions-Rejected To Edit-Count
000320     Display "Transactions Rejected " Edit-Count
000321     Move Total-Commission To Edit-Amt
000322     Display "Total Commission  " Edit-Amt
000323     Stop Run
000325     .
000326
000335 Process-Files.
000345     Evaluate True
000355       When Dealer-Number < Transaction-Dealer
000365            Perform Write-Dealer-Out
000375            Perform Read-Dealer
000385       When Dealer-Number > Transaction-Dealer
000395            Perform Write-Reject
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
000552     Write Dealer-Out-Record From Dealer-Record
000562     .
000563 Write-Reject.
000564     Add 1 To Transactions-Rejected
000565     Write Reject-Record From Trans-Record
000566     .
000572 Read-Dealer.
000582     Read Dealer-File
000592          At End
000602             Move High-Values To Dealer-Record
000612     End-Read
000622     .
000632 Read-Trans.
000642     Read Trans-File
000652          At End
000662             Move High-Values To Trans-Record
000663          Not At End
000664             Add 1 To Transactions-Read
000672     End-Read
000682     .
