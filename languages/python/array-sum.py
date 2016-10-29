#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Задача: в заданном массиве целых чисел найти отрезок с максимальной суммой.
# Ссылка: http://www.cprogramming.com/challenges/array_sum.html
# Массив разрешается просматривать всего один раз.
# Дополнительную память использовать нельзя.
#
array = [3, -2, 1, 4, 5, 2, -7, 3]
#array = [3, -2, 1, 4, 5, 2, -7, 3, -100, 90]
#array = [1, -100, 2, -100, 1]
#array = [-7, -2, -3]

sum = 0
bsum = 0
maxsum = 0
last = -1
first = 0
bottom = 0
maxneg = 0
for i in range (0, len (array)):
	sum += array[i]
	if array[i] > 0:
		if sum - bsum > maxsum:
			first = bottom
			last = i
			maxsum = sum - bsum
	else:
		if sum < bsum:
			bottom = i+1
			bsum = sum
		if maxneg >= 0 or array[i] > maxneg:
			maxneg = array[i]

print "Array:", array
if first <= last:
	print "Solution:", array [first : last+1]
else:
	print "Solution:", [maxneg]
