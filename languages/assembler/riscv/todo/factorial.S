//
// From https://developer.arm.com/documentation/107829/0200/Example--factorial-calculation
//
my_function:
        mov     x3, x0      // Copy register x0 to x3. Use x0 as running total, x3 as counter
loop:
        subs    x3, x3, #1  // Decrement counter in x3
        cmp     x3, #0      // Compare counter to zero
        b.eq    finish      // If result of comparison is "equal" (x3==0), branch to finish
        mul     x0, x0, x3  // Multiply running total and counter (x0*x3), then store in x0
        b       loop        // Branch to do another loop ; keep going until the counter is 0
finish:
        ret                 // Return the running total, which is already in x0
