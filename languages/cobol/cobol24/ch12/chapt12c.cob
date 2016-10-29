000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt12c.
000044 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000067 01  State-Table-Area.
000068     03  State-Table-Data.
000077         05  Filler Pic X(22) Value "ALAlabama".
000087         05  Filler Pic X(22) Value "AKAlaska".
000097         05  Filler Pic X(22) Value "AZArizona".
000107         05  Filler Pic X(22) Value "ARArkansas".
000117         05  Filler Pic X(22) Value "CACalifornia".
000127         05  Filler Pic X(22) Value "COColorado".
000137         05  Filler Pic X(22) Value "CTConnecticut".
000138         05  Filler Pic X(22) Value "DCDistrict of Columbia".
000147         05  Filler Pic X(22) Value "DEDelaware".
000157         05  Filler Pic X(22) Value "FLFlorida".
000158         05  Filler Pic X(22) Value "GAGeorgia".
000159         05  Filler Pic X(22) Value "HIHawaii".
000160         05  Filler Pic X(22) Value "IDIdaho".
000161         05  Filler Pic X(22) Value "ILIllinois".
000162         05  Filler Pic X(22) Value "INIndiana".
000163         05  Filler Pic X(22) Value "IAIowa".
000164         05  Filler Pic X(22) Value "KSKansas".
000165         05  Filler Pic X(22) Value "KYKentucky".
000166         05  Filler Pic X(22) Value "LALouisiana".
000167         05  Filler Pic X(22) Value "MEMaine".
000168         05  Filler Pic X(22) Value "MDMaryland".
000169         05  Filler Pic X(22) Value "MAMassachusetts".
000170         05  Filler Pic X(22) Value "MIMichigan".
000171         05  Filler Pic X(22) Value "MNMinnesota".
000172         05  Filler Pic X(22) Value "MSMississipi".
000173         05  Filler Pic X(22) Value "MOMissouri".
000174         05  Filler Pic X(22) Value "MTMontana".
000175         05  Filler Pic X(22) Value "NENebraska".
000176         05  Filler Pic X(22) Value "NVNevada".
000177         05  Filler Pic X(22) Value "NHNew Hampshire".
000178         05  Filler Pic X(22) Value "NJNew Jersey".
000179         05  Filler Pic X(22) Value "NMNew Mexico".
000180         05  Filler Pic X(22) Value "NYNew York".
000181         05  Filler Pic X(22) Value "NCNorth Carolina".
000182         05  Filler Pic X(22) Value "NDNorth Dakota".
000183         05  Filler Pic X(22) Value "OHOhio".
000184         05  Filler Pic X(22) Value "OKOklahoma".
000185         05  Filler Pic X(22) Value "OROregon".
000186         05  Filler Pic X(22) Value "PAPennsylvania".
000187         05  Filler Pic X(22) Value "RIRhode Island".
000188         05  Filler Pic X(22) Value "SCSouth Carolina".
000189         05  Filler Pic X(22) Value "SDSouth Dakota".
000190         05  Filler Pic X(22) Value "TNTennessee".
000191         05  Filler Pic X(22) Value "TXTexas".
000192         05  Filler Pic X(22) Value "UTUtah".
000193         05  Filler Pic X(22) Value "VTVermont".
000194         05  Filler Pic X(22) Value "VAVirginia".
000195         05  Filler Pic X(22) Value "WAWashington".
000196         05  Filler Pic X(22) Value "WVWest Virginia".
000197         05  Filler Pic X(22) Value "WIWisconsin".
000198         05  Filler Pic X(22) Value "WYWyoming".
000199     03  State-Table Redefines State-Table-Data.
000200         05  State-Table-Occurrences  Occurs 51 Times
000201                                      Indexed By Table-Index.
000202             10  State-Abbrev        Pic XX.
000203             10  State-Name          Pic X(20).
000205 Procedure Division.
000206 Chapt12c-Start.
000214* Search For Texas, By Abbreviation
000215     Search State-Table-Occurrences
000216            At End Display "State Not Found"
000217            When State-Abbrev (Table-Index) = "TX"
000218                 Display "TX = "
000219                         State-Name (Table-Index)
000221     End-Search
000222     Stop Run
000344     .