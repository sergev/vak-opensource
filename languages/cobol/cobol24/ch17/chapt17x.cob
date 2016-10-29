000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt17x.
000031* Chapter 17 Exercise Solution.
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000057 Input-Output Section.
000058 File-Control.
000059     Select Dealer-File Assign To "Dealer.Dat"
000060            Organization Indexed
000061            Record Key Dealer-Number Of Dealer-Record
000062            Alternate Record Key Dealer-Name Of Dealer-Record
000064            Access Sequential
000065            File Status Dealer-Status.
000066     Select Address-File Assign To "Address.Txt"
000067            Organization Line Sequential
000068            Access Sequential
000069            File Status Address-Status.
000070     Select Sort-Work Assign To Dealer-Sort-Work.
000071 Data Division.
000072 File Section.
000073 Fd  Dealer-File.
000074 01  Dealer-Record.
000075     03  Dealer-Number         Pic X(8).
000076     03  Dealer-Name.
000077         05  Last-Name   Pic X(25).
000078         05  First-Name  Pic X(15).
000079         05  Middle-Name Pic X(10).
000083     03  Address-Line-1      Pic X(50).
000085     03  Address-Line-2      Pic X(50).
000086     03  City                Pic X(40).
000087     03  State-Or-Country    Pic X(20).
000088     03  Postal-Code         Pic X(15).
000089     03  Home-Phone          Pic X(20).
000090     03  Work-Phone          Pic X(20).
000091     03  Other-Phone         Pic X(20).
000092     03  Start-Date          Pic 9(8).
000093     03  Last-Rent-Paid-Date Pic 9(8).
000094     03  Next-Rent-Due-Date  Pic 9(8).
000095     03  Rent-Amount         Pic 9(4)v99.
000096     03  Consignment-Percent Pic 9(3).
000097     03  Last-Sold-Amount    Pic S9(7)v99.
000098     03  Last-Sold-Date      Pic 9(8).
000099     03  Sold-To-Date        Pic S9(7)v99.
000100     03  Commission-To-Date  Pic S9(7)v99.
000101     03  Filler              Pic X(15).
000102* Note That The Field Being Sorted On, Sort-State-Name May Appear
000103* Anywhere In The Record.
000104 Sd  Sort-Work.
000105 01  Sort-Record.
000106     03  Dealer-Number       Pic X(8).
000107     03  Sort-State-Name     Pic X(20).
000108     03  Dealer-Name.
000109         05  Last-Name       Pic X(25).
000110         05  First-Name      Pic X(15).
000111         05  Middle-Name     Pic X(10).
000112     03  Address-Line-1      Pic X(50).
000113     03  Address-Line-2      Pic X(50).
000114     03  City                Pic X(40).
000115     03  State-Or-Country    Pic X(20).
000116     03  Postal-Code         Pic X(15).
000124 Fd  Address-File.
000125 01  Address-Record.
000126     03  Dealer-Number       Pic X(8).
000127     03  Dealer-Name.
000128         05  Last-Name       Pic X(25).
000129         05  First-Name      Pic X(15).
000130         05  Middle-Name     Pic X(10).
000131     03  Address-Line-1      Pic X(50).
000132     03  Address-Line-2      Pic X(50).
000133     03  City                Pic X(40).
000134     03  State-Or-Country    Pic X(20).
000135     03  Postal-Code         Pic X(15).
000136 Working-Storage Section.
000138 01  Done-Flag      Pic X Value Spaces.
000148     88  All-Done         Value "Y".
000158 01  Dealer-Status  Pic XX Value "00".
000168 01  Address-Status Pic XX Value "00".
000178 01  Sorted-Records Pic 9(5) Value Zeros.
000188 01  State-Table-Area.
000189     03  State-Table-Data.
000190         05  Filler Pic X(22) Value "ALAlabama".
000191         05  Filler Pic X(22) Value "AKAlaska".
000192         05  Filler Pic X(22) Value "AZArizona".
000193         05  Filler Pic X(22) Value "ARArkansas".
000194         05  Filler Pic X(22) Value "CACalifornia".
000195         05  Filler Pic X(22) Value "COColorado".
000196         05  Filler Pic X(22) Value "CTConnecticut".
000197         05  Filler Pic X(22) Value "DCDistrict of Columbia".
000198         05  Filler Pic X(22) Value "DEDelaware".
000199         05  Filler Pic X(22) Value "FLFlorida".
000200         05  Filler Pic X(22) Value "GAGeorgia".
000201         05  Filler Pic X(22) Value "HIHawaii".
000202         05  Filler Pic X(22) Value "IDIdaho".
000203         05  Filler Pic X(22) Value "ILIllinois".
000204         05  Filler Pic X(22) Value "INIndiana".
000205         05  Filler Pic X(22) Value "IAIowa".
000206         05  Filler Pic X(22) Value "KSKansas".
000207         05  Filler Pic X(22) Value "KYKentucky".
000208         05  Filler Pic X(22) Value "LALouisiana".
000209         05  Filler Pic X(22) Value "MEMaine".
000210         05  Filler Pic X(22) Value "MDMaryland".
000211         05  Filler Pic X(22) Value "MAMassachusetts".
000212         05  Filler Pic X(22) Value "MIMichigan".
000213         05  Filler Pic X(22) Value "MNMinnesota".
000214         05  Filler Pic X(22) Value "MSMississipi".
000215         05  Filler Pic X(22) Value "MOMissouri".
000216         05  Filler Pic X(22) Value "MTMontana".
000217         05  Filler Pic X(22) Value "NENebraska".
000218         05  Filler Pic X(22) Value "NVNevada".
000219         05  Filler Pic X(22) Value "NHNew Hampshire".
000220         05  Filler Pic X(22) Value "NJNew Jersey".
000221         05  Filler Pic X(22) Value "NMNew Mexico".
000222         05  Filler Pic X(22) Value "NYNew York".
000223         05  Filler Pic X(22) Value "NCNorth Carolina".
000224         05  Filler Pic X(22) Value "NDNorth Dakota".
000225         05  Filler Pic X(22) Value "OHOhio".
000226         05  Filler Pic X(22) Value "OKOklahoma".
000227         05  Filler Pic X(22) Value "OROregon".
000228         05  Filler Pic X(22) Value "PAPennsylvania".
000229         05  Filler Pic X(22) Value "RIRhode Island".
000230         05  Filler Pic X(22) Value "SCSouth Carolina".
000231         05  Filler Pic X(22) Value "SDSouth Dakota".
000232         05  Filler Pic X(22) Value "TNTennessee".
000233         05  Filler Pic X(22) Value "TXTexas".
000234         05  Filler Pic X(22) Value "UTUtah".
000235         05  Filler Pic X(22) Value "VTVermont".
000236         05  Filler Pic X(22) Value "VAVirginia".
000237         05  Filler Pic X(22) Value "WAWashington".
000238         05  Filler Pic X(22) Value "WVWest Virginia".
000239         05  Filler Pic X(22) Value "WIWisconsin".
000240         05  Filler Pic X(22) Value "WYWyoming".
000241     03  State-Table Redefines State-Table-Data.
000242         05  State-Table-Occurrences  Occurs 51 Times
000243                                     Indexed By Table-Index.
000244             10  State-Abbrev        Pic XX.
000245             10  State-Name          Pic X(20).
000390 Procedure Division.
000391 Declaratives.
000392 Dealer-File-Error Section.
000393     Use After Standard Error Procedure On Dealer-File.
000394 Dealer-Error.
000395     Display "Unhandled error on Dealer File " Dealer-Status
000396     Set All-Done To True
000397     .
000398 Address-File-Error Section.
000399     Use After Standard Error Procedure On Address-File.
000400 Address-Error.
000401     Display "Unhandled error on Address File " Address-Status
000402     Set All-Done To True
000403     .
000404 End Declaratives.
000405 Chapt17x-Start.
000406     Display "Begin Sort Chapt17x"
000407* Duplicates Is Used Here To Great Advantage.  Since The Indexed File
000408* Is Input, It Is Already In Dealer Number Sequence.  By Specifying
000409* Duplicates, We End Up With The Output Being In Dealer Number Sequence
000410* Within Postal Code In The Output File.
000411     Sort Sort-Work Ascending  Key Sort-State-Name
000412                                   Postal-Code Of Sort-Record
000413          Duplicates
000431          Input Procedure Sort-In
000441          Output Procedure Sort-Out
000461     Display "Sort Complete with " Sorted-Records " Records."
000471     Stop Run
000481     .
000491 Sort-In.
000492     Open Input Dealer-File
000501     Perform Until All-Done
000502        Read Dealer-File
000503             At End Set All-Done To True
000504             Not At End
000505             Move Corresponding Dealer-Record To Sort-Record
000507             Perform Find-State-Name
000508             Release Sort-Record
000510        End-Read
000511     End-Perform
000512     Close Dealer-File
000521     .
000522 Sort-Out.
000523* The Flag Is Set From The Sort In Procedure, Reset It.
000525     Move Spaces To Done-Flag
000526     Open Output Address-File
000527     Perform Until All-Done
000528        Return Sort-Work
000529          At End Set All-Done To True
000530          Not At End
000531             Move Corresponding Sort-Record To Address-Record
000532             Write Address-Record
000533             Add 1 To Sorted-Records
000534        End-Return
000535     End-Perform
000536     .
000537 Find-State-Name.
000541     Set Table-Index To 1
000551     Search State-Table-Occurrences
000561            At End Move "UNKNOWN" To Sort-State-Name
000571            When State-Abbrev (Table-Index) =
000572                 State-Or-Country Of Sort-Record
000573                 Move State-Name (Table-Index) To
000574                      Sort-State-Name
000581     End-Search
000611     .