000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22x.
000031*chapter 22 Exercise Solution
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000058 01  Random-Seed       Pic 9(8)         Value Zeros.
000059 01  Counter           Pic 9(4)         Value Zeros.
000069 01  Random-Number-Table.
000079     03  Random-Number Pic 9(4) Occurs 3000 Times.
000089 01  Random-Generate   Pic V9(18)       Value Zeros.
000090 01  Result            Pic 9(5)v9(4)    Value Zeros.
000099 01  Ed-Result         Pic ZZ,ZZZ.9999.
000148 Procedure Division.
000159 Chapt22x-Start.
000160     Display "Starting Random Number Generation"
000161     Move Function Current-Date (11:8) To Random-Seed
000162     Compute Random-Generate = Function Random (Random-Seed)
000163     Compute Random-Number (1) =
000164             (Random-Generate * 1000) + 1
000165     Perform Varying Counter From 2 By 1
000166             Until Counter > 3000
000167         Compute Random-Generate = Function Random
000168         Compute Random-Number (Counter) =
000169             (Random-Generate * 1000) + 1
000170     End-Perform
000171     Display "Random Number Generation Complete"
000172     Compute Result = Function Min (Random-Number (All))
000173     Move Result To Ed-Result
000174     Display "Min=" Ed-Result
000175     Compute Result = Function Max (Random-Number (All))
000176     Move Result To Ed-Result
000177     Display "Max=" Ed-Result
000178     Compute Result = Function Mean (Random-Number (All))
000179     Move Result To Ed-Result
000180     Display "Mean=" Ed-Result
000181     Compute Result = Function Median (Random-Number (All))
000182     Move Result To Ed-Result
000183     Display "Median=" Ed-Result
000184     Compute Result = Function Midrange (Random-Number (All))
000185     Move Result To Ed-Result
000186     Display "Midrange=" Ed-Result
000187     Compute Result = Function Range (Random-Number (All))
000188     Move Result To Ed-Result
000189     Display "Range=" Ed-Result
000190     Compute Result = Function Variance (Random-Number (All))
000191     Move Result To Ed-Result
000192     Display "Variance=" Ed-Result
000193     Compute Result = Function
000194             Standard-Deviation (Random-Number (All))
000195     Move Result To Ed-Result
000196     Display "Standard Deviation=" Ed-Result
000197     Stop Run
000198     .