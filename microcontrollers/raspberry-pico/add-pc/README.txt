Result:

    n = 0, abcd = 0 1 1 1
    n = 2, abcd = 0 0 1 1
    n = 4, abcd = 0 0 0 1
    n = 6, abcd = 0 0 0 0

Disassembled code:

    10000352:       2200            movs    r2, #0
    10000354:       2300            movs    r3, #0
    10000356:       2400            movs    r4, #0
    10000358:       2500            movs    r5, #0
    1000035a:       4487            add     pc, r0
    1000035c:       2201            movs    r2, #1
    1000035e:       2301            movs    r3, #1
    10000360:       2401            movs    r4, #1
    10000362:       2501            movs    r5, #1
