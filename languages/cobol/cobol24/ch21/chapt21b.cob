000010 @OPTIONS MAIN,TEST
000020 Identification Division.
000030 Program-Id.  Chapt21b.
000041 Environment Division.
000050 Configuration Section.
000051 Source-Computer.  IBM-PC.
000056 Object-Computer.  IBM-PC.
000147 Data Division.
000148 Working-Storage Section.
000390 01  Integer-Version-Of-Date   Pic 9(7) Value Zeros.
000391 01  Date-To-Convert           Pic 9(8) Value 19991231.
000392 Procedure Division.
000393 Chapt21b.
000401     Compute Integer-Version-Of-Date =
000402             Function Integer-Of-Date (Date-To-Convert)
000403     Display "Integer Date Version of " Date-To-Convert
000404             " is " Integer-Version-Of-Date
000421     .