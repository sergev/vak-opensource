000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt18d.
000031* Indexed File Update
000043 Environment Division.
000050 Configuration Section.
000055 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000061 Input-Output  Section.
000062 File-Control.
000063     Select Optional Trans-File Assign To "Trans.Txt"
000064         Organization Is Line Sequential.
000066     Select Optional Dealer-File Assign To "Dealer.Dat"
000067         Organization Indexed
000068         Access Random
000069         Record Key Dealer-Number
000070         Alternate Record Key Dealer-Name
000071         File Status Dealer-Status.
000074     Select Optional Reject-File Assign To "Reject.Txt"
000075         Organization Is Line Sequential.
000076 Data Division.
000077 File Section.
000078 Fd  Trans-File.
000079 01  Trans-Record.
000080     03  Transaction-Date   Pic  9(8).
000081     03  Transaction-Text.
000082         05  Transaction-Type   Pic  X(4).
000083         05  Transaction-Dealer Pic  X(8).
000084     03  Transaction-Price  Pic S9(7)v99.
000085     03  Transaction-Qty    Pic  9(3).
000086     03  Filler             Pic  X(40).
000087 Fd  Reject-File.
000088 01  Reject-Record          Pic X(72).
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
000202 01  Used-Transaction-Flag   Pic X Value Spaces.
000203     88  Used-This-Tran      Value "Y".
000204 01  Edit-Count              Pic ZZ,ZZ9.
000205 01  Edit-Amt                Pic Z,ZZZ,ZZZ.99-.
000206 01  Dealer-Status           Pic XX Value Zeros.
000207     88  Dealer-Success  Value "00" Thru "09".
000208 01  Trans-Flag              Pic X Value Spaces.
000209     88  End-Of-Trans        Value "Y".
000211 01  Dealer-Flag             Pic X Value Spaces.
000212     88  Dealer-Error        Value "Y".
000213 Procedure Division.
000214 Declaratives.
000215 Dealer-File-Error Section.
000216     Use After Standard Error Procedure On Dealer-File
000217     .
000218 Dealer-Error-Paragraph.
000219     Display "Error on Dealer File " Dealer-Status
000220     Set Dealer-Error To True
000221     .
000222 End Declaratives.
000223 Chapt18d-Start.
000224     Display "Begin Process Chapt18d"
000225     Open Output Reject-File
000235          Input  Trans-File
000245          I-O    Dealer-File
000255     Perform Process-Files Until End-Of-Trans Or Dealer-Error
000265     Close Reject-File
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
000336     Read Trans-File
000337          At End Set End-Of-Trans To True
000338          Not At End
000339              Add 1 To Transactions-Read
000340              Perform Attempt-Transaction
000341     End-Read
000485     .
000495 Attempt-Transaction.
000496     Move Transaction-Dealer To Dealer-Number
000497     Read Dealer-File
000498          Invalid Key
000499            Perform Write-Reject
000500          Not Invalid Key
000501            Perform Apply-Transaction
000502     End-Read
000503     .
000504 Apply-Transaction.
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
000541     Rewrite Dealer-Record
000542     .
000563 Write-Reject.
000564     Add 1 To Transactions-Rejected
000565     Write Reject-Record From Trans-Record
000566     .
