    .text
    .align  2           // Make sure everything is aligned properly
    .global _start
_start:

    // Condition codes: N=0 Z=0 C=0 V=0
    ldr X1, =0x00000000
    msr NZCV, X1
    beq fail
    bne 1f
    b fail
1:
    bcs fail
    bcc 2f
    b fail
2:
    bmi fail
    bpl 3f
    b fail
3:
    bvs fail
    bvc 4f
    b fail
4:
    bhi fail
    bls 5f
    b fail
5:
    bge 6f
    b fail
6:
    blt fail
    bgt 7f
    b fail
7:
    ble fail

    // Condition codes: N=0 Z=0 C=0 V=1
    ldr X1, =0x10000000
    msr NZCV, X1
    beq fail
    bne 8f
    b fail
8:
    bcs fail
    bcc 9f
    b fail
9:
    bmi fail
    bpl 10f
    b fail
10:
    bvs 11f
    b fail
11:
    bvc fail
    bhi fail
    bls 12f
    b fail
12:
    bge fail
    blt 13f
    b fail
13:
    bgt fail
    ble 14f
    b fail
14:

    // Condition codes: N=0 Z=0 C=1 V=0
    ldr X1, =0x20000000
    msr NZCV, X1
    beq fail
    bne 15f
    b fail
15:
    bcs 16f
    b fail
16:
    bcc fail
    bmi fail
    bpl 17f
    b fail
17:
    bvs fail
    bvc 18f
    b fail
18:
    bhi 19f
    b fail
19:
    bls fail
    bge 20f
    b fail
20:
    blt fail
    bgt 21f
    b fail
21:
    ble fail

    // Condition codes: N=0 Z=0 C=1 V=1
    ldr X1, =0x30000000
    msr NZCV, X1
    beq fail
    bne 22f
    b fail
22:
    bcs 23f
    b fail
23:
    bcc fail
    bmi fail
    bpl 24f
    b fail
24:
    bvs 25f
    b fail
25:
    bvc fail
    bhi 26f
    b fail
26:
    bls fail
    bge fail
    blt 27f
    b fail
27:
    bgt fail
    ble 28f
    b fail
28:

    // Condition codes: N=0 Z=1 C=0 V=0
    ldr X1, =0x40000000
    msr NZCV, X1
    beq 29f
    b fail
29:
    bne fail
    bcs fail
    bcc 30f
    b fail
30:
    bmi fail
    bpl 31f
    b fail
31:
    bvs fail
    bvc 32f
    b fail
32:
    bhi fail
    bls 33f
    b fail
33:
    bge 34f
    b fail
34:
    blt fail
    bgt fail
    ble 35f
    b fail
35:

    // Condition codes: N=0 Z=1 C=0 V=1
    ldr X1, =0x50000000
    msr NZCV, X1
    beq 36f
    b fail
36:
    bne fail
    bcs fail
    bcc 37f
    b fail
37:
    bmi fail
    bpl 38f
    b fail
38:
    bvs 39f
    b fail
39:
    bvc fail
    bhi fail
    bls 40f
    b fail
40:
    bge fail
    blt 41f
    b fail
41:
    bgt fail
    ble 42f
    b fail
42:

    // Condition codes: N=0 Z=1 C=1 V=0
    ldr X1, =0x60000000
    msr NZCV, X1
    beq 43f
    b fail
43:
    bne fail
    bcs 44f
    b fail
44:
    bcc fail
    bmi fail
    bpl 45f
    b fail
45:
    bvs fail
    bvc 46f
    b fail
46:
    bhi fail
    bls 47f
    b fail
47:
    bge 48f
    b fail
48:
    blt fail
    bgt fail
    ble 49f
    b fail
49:

    // Condition codes: N=0 Z=1 C=1 V=1
    ldr X1, =0x70000000
    msr NZCV, X1
    beq 50f
    b fail
50:
    bne fail
    bcs 51f
    b fail
51:
    bcc fail
    bmi fail
    bpl 52f
    b fail
52:
    bvs 53f
    b fail
53:
    bvc fail
    bhi fail
    bls 54f
    b fail
54:
    bge fail
    blt 55f
    b fail
55:
    bgt fail
    ble 56f
    b fail
56:

    // Condition codes: N=1 Z=0 C=0 V=0
    ldr X1, =0x80000000
    msr NZCV, X1
    beq fail
    bne 57f
    b fail
57:
    bcs fail
    bcc 58f
    b fail
58:
    bmi 59f
    b fail
59:
    bpl fail
    bvs fail
    bvc 60f
    b fail
60:
    bhi fail
    bls 61f
    b fail
61:
    bge fail
    blt 62f
    b fail
62:
    bgt fail
    ble 63f
    b fail
63:

    // Condition codes: N=1 Z=0 C=0 V=1
    ldr X1, =0x90000000
    msr NZCV, X1
    beq fail
    bne 64f
    b fail
64:
    bcs fail
    bcc 65f
    b fail
65:
    bmi 66f
    b fail
66:
    bpl fail
    bvs 67f
    b fail
67:
    bvc fail
    bhi fail
    bls 68f
    b fail
68:
    bge 69f
    b fail
69:
    blt fail
    bgt 70f
    b fail
70:
    ble fail

    // Condition codes: N=1 Z=0 C=1 V=0
    ldr X1, =0xa0000000
    msr NZCV, X1
    beq fail
    bne 71f
    b fail
71:
    bcs 72f
    b fail
72:
    bcc fail
    bmi 73f
    b fail
73:
    bpl fail
    bvs fail
    bvc 74f
    b fail
74:
    bhi 75f
    b fail
75:
    bls fail
    bge fail
    blt 76f
    b fail
76:
    bgt fail
    ble 77f
    b fail
77:

    // Condition codes: N=1 Z=0 C=1 V=1
    ldr X1, =0xb0000000
    msr NZCV, X1
    beq fail
    bne 78f
    b fail
78:
    bcs 79f
    b fail
79:
    bcc fail
    bmi 80f
    b fail
80:
    bpl fail
    bvs 81f
    b fail
81:
    bvc fail
    bhi 82f
    b fail
82:
    bls fail
    bge 83f
    b fail
83:
    blt fail
    bgt 84f
    b fail
84:
    ble fail

    // Condition codes: N=1 Z=1 C=0 V=0
    ldr X1, =0xc0000000
    msr NZCV, X1
    beq 85f
    b fail
85:
    bne fail
    bcs fail
    bcc 86f
    b fail
86:
    bmi 87f
    b fail
87:
    bpl fail
    bvs fail
    bvc 88f
    b fail
88:
    bhi fail
    bls 89f
    b fail
89:
    bge fail
    blt 90f
    b fail
90:
    bgt fail
    ble 91f
    b fail
91:

    // Condition codes: N=1 Z=1 C=0 V=1
    ldr X1, =0xd0000000
    msr NZCV, X1
    beq 92f
    b fail
92:
    bne fail
    bcs fail
    bcc 93f
    b fail
93:
    bmi 94f
    b fail
94:
    bpl fail
    bvs 95f
    b fail
95:
    bvc fail
    bhi fail
    bls 96f
    b fail
96:
    bge 97f
    b fail
97:
    blt fail
    bgt fail
    ble 98f
    b fail
98:

    // Condition codes: N=1 Z=1 C=1 V=0
    ldr X1, =0xe0000000
    msr NZCV, X1
    beq 99f
    b fail
99:
    bne fail
    bcs 100f
    b fail
100:
    bcc fail
    bmi 101f
    b fail
101:
    bpl fail
    bvs fail
    bvc 102f
    b fail
102:
    bhi fail
    bls 103f
    b fail
103:
    bge fail
    blt 104f
    b fail
104:
    bgt fail
    ble 105f
    b fail
105:

    // Condition codes: N=1 Z=1 C=1 V=1
    ldr X1, =0xf0000000
    msr NZCV, X1
    beq 106f
    b fail
106:
    bne fail
    bcs 107f
    b fail
107:
    bcc fail
    bmi 108f
    b fail
108:
    bpl fail
    bvs 109f
    b fail
109:
    bvc fail
    bhi fail
    bls 110f
    b fail
110:
    bge 111f
    b fail
111:
    blt fail
    bgt fail
    ble 112f
    b fail
112:
    mov X0, #1          // Print to StdOut
    adr X1, passed      // Put address of text message into register X1
    mov X2, #12         // Length of our string
    mov X16, #4         // Unix write() system call
    svc #0x80           // Call kernel to output the string
    mov X0, #0          // Exit code
    mov X16, #1         // Unix _exit() system call
    svc #0x80           // Call kernel to terminate the program
passed:
    .ascii "Test passed\n"
fail:
    mov X0, #1          // Print to StdOut
    adr X1, failed      // Put address of text message into register X1
    mov X2, #12         // Length of our string
    mov X16, #4         // Unix write() system call
    svc #0x80           // Call kernel to output the string
    mov X0, #255        // Exit code
    mov X16, #1         // Unix _exit() system call
    svc #0x80           // Call kernel to terminate the program
failed:
    .ascii "Test failed\n"
