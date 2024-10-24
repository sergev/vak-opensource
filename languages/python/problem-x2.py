#!/usr/bin/env python3
#
# Find a natural number that doubles after moving
# its last digit to the beginning of the number.
#
for n in range(1, 60):
    x = 10**n * 2 - 4
    if x % 19 == 0:
        print("solution: = %d2" % (x // 19))

# Output:
# solution: = 105263157894736842
# solution: = 105263157894736831521933768712519682
# solution: = 105263157894736835794929351891051901085202740076871682
