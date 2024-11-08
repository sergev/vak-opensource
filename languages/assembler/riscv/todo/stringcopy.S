section .text
.global stringcopy
stringcopy:
    # a0 = destination
    # a1 = source
1:
    lb      t0, 0(a1)    # Load a char from the src
    sb      t0, 0(a0)    # Store the value of the src
    beqz    t0, 1f       # Check if it's 0
    addi    a0, a0, 1    # Advance destination one byte
    addi    a1, a1, 1    # Advance source one byte
    j       1b           # Go back to the start of the loop
1:
    ret                  # Return back via the return address
