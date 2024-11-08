.section .text
.global strrev
strrev:
    # s1 = str
    # a0 = sz
    # t0 = sz / 2
    # t1 = i
    # Enter stack frame
    addi    sp, sp, -16
    sd      ra, 0(sp)
    sd      s1, 8(sp)

    # Get the size of the string
    mv      s1, a0
    call    strlen
    srai    t0, a0, 1     # Divide sz by 2
    li      t1, 0         # i = 0
1:  # for loop
    bge     t1, t0, 1f
    add     t2, s1, t1    # str + i
    sub     t3, a0, t1    # sz - i
    addi    t3, t3, -1    # sz - i - 1
    add     t3, t3, s1    # str + sz - i - 1
    lb      t4, 0(t2)     # str[i]
    lb      t5, 0(t3)     # str[sz - i - 1]
    sb      t4, 0(t3)     # swap
    sb      t5, 0(t2)
    addi    t1, t1, 1
    j       1b
1:
    # Leave stack frame
    ld      s1, 8(sp)
    ld      ra, 0(sp)
    addi    sp, sp, 16
    ret
