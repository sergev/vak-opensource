000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt22c.
000031* Present Value.
000040 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000055 Object-Computer.  IBM-PC.
000056 Data Division.
000057 Working-Storage Section.
000107 01  Result         Pic 9(6)v99 Value Zeros.
000148 Procedure Division.
000159 Chapt22c-Start.
000199     Compute Result = Function Present-Value (.05 1000)
000209     Display "Result = " Result
000219* 100 Dollars Every Year For 20 Years:
000229     Compute Result = Function Present-Value (.045,
000239             100, 100, 100, 100, 100
000249             100, 100, 100, 100, 100
000259             100, 100, 100, 100, 100
000269             100, 100, 100, 100, 100)
000279     Display "Result 1 = " Result
000309     Stop Run.
000319     .