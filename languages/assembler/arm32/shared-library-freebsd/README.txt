Unfortunately, this code cannot be traced on FreeBSD with bintrace,
as it loops forever in a fragment:

    0x2003202c:  e1923f9f   ldrex r3, [r2]      -- tag the physical address as exclusive access for the current processor
            r3 = 0
    0x20032030:  e0833001   add r3, r3, r1
            r3 = 0x10
    0x20032034:  e1820f93   strex r0, r3, [r2]  -- check for exclusive access, otherwize set r0=1
    0x20032038:  e3500000   cmp r0, #0
    0x2003203c:  1afffffa   bne #0x2003202c
