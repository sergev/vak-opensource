000010 @OPTIONS MAIN
000020 Identification Division.
000030 Program-Id.  Chapt09e.
000031* Complex If Vs Evaluate Example 3
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Entry-Fields.
000059     03  Month-Of-Sale        Pic 99      Value 12.
000060     03  Category-Of-Sale     Pic X(4)    Value "JEWL".
000062     03  Sale-Item-Flag       Pic X       Value "Y".
000063         88  Sale-Item                    Value "Y".
000064     03  Full-Price           Pic 9(4)v99 Value 120.00.
000065     03  Sale-Price           Pic 9(4)v99 Value Zeros.
000066     03  Discount-Percent     Pic 999.
000072 Procedure Division.
000159 Chapt09e-Start.
000160     If Sale-Item
000161        If Month-Of-Sale = 01 Or 02 Or 03
000162           If Category-Of-Sale = "ANTI" Or "JEWL" Or "MISC"
000163              Move 50 To Discount-Percent
000164              Compute Sale-Price = Full-Price * .5
000165           Else
000166              If Category-Of-Sale = "XMAS" Or "CRAF"
000167                 Move 75 To Discount-Percent
000168                 Compute Sale-Price = Full-Price * .25
000169              Else
000170                 Move 10 To Discount-Percent
000171                 Compute Sale-Price = Full-Price * .90
000172              End-If
000173           End-If
000174        Else
000175           If Month-Of-Sale = 04 Or 05 Or 06
000176              If Category-Of-Sale = "XMAS" Or "CRAF"
000177                 Move 50 To Discount-Percent
000178                 Compute Sale-Price = Full-Price * .5
000179              Else
000180                 If Category-Of-Sale = "ANTI" Or "JEWL" Or "MISC"
000181                    Move 25 To Discount-Percent
000182                    Compute Sale-Price = Full-Price * .75
000183                 Else
000184                    Move 10 To Discount-Percent
000185                    Compute Sale-Price = Full-Price * .90
000186                 End-If
000187              End-If
000188           Else
000189              If Month-Of-Sale = 07 Or 08 Or 09
000190                 Move 25 To Discount-Percent
000191                 Compute Sale-Price = Full-Price *  .75
000192              Else
000193                 If Category-Of-Sale = "ANTI"
000194                    Move 50 To Discount-Percent
000195                    Compute Sale-Price = Full-Price * .5
000196                 Else
000197                    Move 10 To Discount-Percent
000198                    Compute Sale-Price = Full-Price * .9
000199                 End-If
000200              End-If
000201           End-If
000202        End-If
000203     Else
000204        Move Full-Price To Sale-Price
000205     End-If
000206     Display "Full Price " Full-Price
000207     Display "Sale Price " Sale-Price
000208     Evaluate Sale-Item Also Month-Of-Sale Also Category-Of-Sale
000209        When True Also 1 Thru 3 Also "ANTI"
000210        When True Also 1 Thru 3 Also "JEWL"
000211        When True Also 1 Thru 3 Also "MISC"
000212             Move 50 To Discount-Percent
000213             Compute Sale-Price = Full-Price * .5
000214        When True Also 1 Thru 3 Also "XMAS"
000215        When True Also 1 Thru 3 Also "CRAF"
000216             Move 75 To Discount-Percent
000217             Compute Sale-Price = Full-Price * .25
000218        When True Also 1 Thru 3 Also Any
000219             Move 10 To Discount-Percent
000220             Compute Sale-Price = Full-Price * .9
000221        When True Also 4 Thru 6 Also "XMAS"
000222        When True Also 4 Thru 6 Also "CRAF"
000223             Move 50 To Discount-Percent
000224             Compute Sale-Price = Full-Price * .5
000225        When True Also 4 Thru 6 Also "ANTI"
000226        When True Also 4 Thru 6 Also "JEWL"
000227        When True Also 4 Thru 6 Also "MISC"
000228             Move 25 To Discount-Percent
000229             Compute Sale-Price = Full-Price * .75
000230        When True Also 4 Thru 6 Also Any
000231             Move 10 To Discount-Percent
000232             Compute Sale-Price = Full-Price * .90
000233        When True Also 6 Thru 9 Also Any
000234             Move 25 To Discount-Percent
000235             Compute Sale-Price = Full-Price * .75
000236        When True Also 10 Thru 12 Also "ANTI"
000237             Move 50 To Discount-Percent
000238             Compute Sale-Price = Full-Price * .5
000239        When True Also 10 Thru 12 Also Any
000240             Move 10 To Discount-Percent
000241             Compute Sale-Price = Full-Price * .9
000242        When Other
000243             Move Full-Price To Sale-Price
000244     End-Evaluate
000247     Display "Full Price " Full-Price
000248     Display "Sale Price " Sale-Price
000249     Evaluate Sale-Item Also Month-Of-Sale Also Category-Of-Sale
000250        When True Also 1 Thru 3 Also "ANTI"
000251        When True Also 1 Thru 3 Also "JEWL"
000252        When True Also 1 Thru 3 Also "MISC"
000253        When True Also 4 Thru 6 Also "XMAS"
000254        When True Also 4 Thru 6 Also "CRAF"
000255        When True Also 10 Thru 12 Also "ANTI"
000257             Move 50 To Discount-Percent
000258             Compute Sale-Price = Full-Price * .5
000259        When True Also 1 Thru 3 Also "XMAS"
000260        When True Also 1 Thru 3 Also "CRAF"
000261             Move 75 To Discount-Percent
000262             Compute Sale-Price = Full-Price * .25
000269        When True Also 4 Thru 6 Also "ANTI"
000270        When True Also 4 Thru 6 Also "JEWL"
000271        When True Also 4 Thru 6 Also "MISC"
000272        When True Also 6 Thru 9 Also Any
000274             Move 25 To Discount-Percent
000275             Compute Sale-Price = Full-Price * .75
000277        When True Also 1 Thru 3 Also Any
000278        When True Also 4 Thru 6 Also Any
000279        When True Also 10 Thru 12 Also Any
000281             Move 10 To Discount-Percent
000282             Compute Sale-Price = Full-Price * .9
000291        When Other
000292             Move Full-Price To Sale-Price
000293     End-Evaluate
000294     Display "Full Price " Full-Price
000295     Display "Sale Price " Sale-Price
000297     Stop Run
000298     .