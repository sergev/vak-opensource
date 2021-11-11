#!/usr/bin/env python3
#
# Represent numbers as list of digits
#
def xprint(num, end='\n'):
    if len(num) > 1:
        xprint(num[1:], end='')
    if len(num) > 0:
        print(num[0], end=end)

#xprint([])        # 0
#xprint([6])       # 6
#xprint([1, 2, 3]) # 321

#base_numerator, base_denominator = 10, 1
#base_numerator, base_denominator = 3, 2
base_numerator, base_denominator = 10, 7

#
# Increment the number by 1.
#
def xincrement(num, incr=1):
    if len(num) < 1:
        return [incr]

    digit = num[0] + incr
    if digit < base_numerator:
        return [digit] + num[1:]

    return [digit - base_numerator] + xincrement(num[1:], base_denominator)

# For base = 10/1
#xprint(xincrement([0]))       # 1
#xprint(xincrement([1]))       # 2
#xprint(xincrement([9]))       # 10
#xprint(xincrement([1, 9]))    # 92
#xprint(xincrement([9, 1]))    # 20
#xprint(xincrement([9, 9, 1])) # 200

# For base = 3/2
#xprint(xincrement([0]))       # 1
#xprint(xincrement([1]))       # 2
#xprint(xincrement([2]))       # 20
#xprint(xincrement([1, 2]))    # 22
#xprint(xincrement([2, 1]))    # 200
#xprint(xincrement([2, 2, 1])) # 2010

x = [0]
for i in range(100):
    x = xincrement(x)
xprint(x)
