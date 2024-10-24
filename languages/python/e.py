#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Вычисление числа e.
#
import math

# e = 1 + 1 + 1/2 + 1/2/3 + 1/2/3/4 + ...
#
def e1():
	r2 = 1./2
	r3 = r2 / 3
	r4 = r3 / 4
	r5 = r4 / 5
	r6 = r5 / 6
	r7 = r6 / 7
	r8 = r7 / 8
	r9 = r8 / 9
	r10 = r9 / 10
	r11 = r10 / 11
	r12 = r11 / 12
	r13 = r12 / 13
	r14 = r13 / 14
	r15 = r14 / 15
	r16 = r15 / 16
	r17 = r16 / 17
	r18 = r17 / 18
	r19 = r18 / 19
	r20 = r19 / 20
	e = r20 + r19
	e += r18 + r17
	e += r16 + r15
	e += r14 + r13
	e += r12 + r11
	e += r10 + r9
	e += r8 + r7
	e += r6 + r5
	e += r4 + r3
	e += r2 + 2
	return e

e_fact = e1()
print(e_fact)
