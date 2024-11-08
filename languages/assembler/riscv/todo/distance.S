.section .text
.global distance
distance:
    # a0 = Coordinate &from
    # a1 = Coordinate &to
    # Coordinate structure
    # Name    Offset     Size (bytes)
    # x       0          8
    # y       8          8
    # Total size = 16 bytes
    fld   ft0, 0(a0)      # ft0 = from.x
    fld   ft1, 8(a0)      # ft1 = from.y
    fld   ft2, 0(a1)      # ft2 = to.x
    fld   ft3, 8(a1)      # ft3 = to.y

    fsub.d  ft0, ft2, ft0 # ft0 = to.x - from.x
    fsub.d  ft1, ft3, ft1 # ft1 = to.y - from.y
    fmul.d  ft0, ft0, ft0 # ft0 = ft0 * ft0
    fmul.d  ft1, ft1, ft1 # ft1 = ft1 * ft1
    fadd.d  ft0, ft0, ft1 # ft0 = ft0 + ft1
    fsqrt.d fa0, ft0      # fa0 = sqrt(ft0)
    # Return value goes in fa0
    ret                   # Return
