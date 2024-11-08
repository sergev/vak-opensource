// void matmul(float dst[3], float matrix[3][3], float vector[3]) {
//     int r;
//     int c;
//     float d;
//     for (r = 0;r < 3;r++) {
//         d = 0;
//         for (c = 0;c < 3;c++) {
//              d = d + matrix[r][c] * vector[c];
//         }
//         dst[r] = d;
//     }
// }
.section .text
.global matmul
matmul:
    # a0 = dst[3]
    # a1 = matrix[3][3]
    # a2 = vector[3]
    # t0 = r
    # t1 = c
    # t3 = 3
    # ft0 = d
    # Row for loop
    li      t0, 0
    li      t3, 3
1:
    bge     t0, t3, 1f  # break when we are done
    fmv.s.x fa0, zero   # Set d = 0
    # Column for loop
    li      t1, 0
2:
    bge     t1, t3, 2f
    flw     ft0, 0(a1)     # Load matrix value
    flw     ft1, 0(a2)     # Load vector value
    fmul.s  ft0, ft0, ft1  # ft0 = matrix[r][c] * vec[c]
    fadd.s  fa0, fa0, ft0  # d = d + ft0
    addi    t1, t1, 1
    addi    a1, a1, 4   # Move to the next matrix value
    addi    a2, a2, 4   # Move to the next vector value
    j       2b
2:
    addi    a2, a2, -12 # Move the vector back to the top
    fsw     fa0, 0(a0)  # dst[r] = d
    addi    t0, t0, 1
    addi    a0, a0, 4   # Move to next destination
    j       1b
1:
    ret
