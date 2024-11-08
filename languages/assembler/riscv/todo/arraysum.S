.section .text
.global arraysum
arraysum:
    # a0 = int a[]
    # a1 = int size
    # t0 = ret
    # t1 = i
    li    t0, 0        # ret = 0
    li    t1, 0        # i = 0
1:  # For loop
    bge   t1, a1, 1f   # if i >= size, break
    slli  t2, t1, 2    # Multiply i by 4 (1 << 2 = 4)
    add   t2, a0, t2   # Update memory address
    lw    t2, 0(t2)    # Dereference address to get integer
    add   t0, t0, t2   # Add integer value to ret
    addi  t1, t1, 1    # Increment the iterator
    j     1b           # Jump back to start of loop (1 backwards)
1:
    mv    a0, t0       # Move t0 (ret) into a0
    ret                # Return via return address register
