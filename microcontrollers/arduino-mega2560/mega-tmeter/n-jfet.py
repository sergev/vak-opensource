#!/usr/bin/python
#
# Compute parameters of N-JFET transistor.
#
import math
import numpy as np

data = [
# Vg, V   Id, mA
 -0.005,  3.841,
 -0.020,  3.555,
 -0.044,  3.168,
 -0.059,  2.877,
 -0.079,  2.586,
 -0.098,  2.300,
 -0.118,  2.105,
 -0.142,  1.818,
 -0.162,  1.623,
 -0.181,  1.336,
 -0.201,  1.236,
 -0.220,  1.045,
 -0.235,  0.850,
 -0.259,  0.755,
 -0.274,  0.659,
 -0.293,  0.564,
 -0.313,  0.564,
 -0.337,  0.468,
 -0.352,  0.468,
 -0.376,  0.368,
 -0.396,  0.368,
 -0.411,  0.368,
]

#
# Copy data into Numpy arrays.
#
N = len(data) / 2
Vg = np.empty(N)
Id = np.empty(N)
for i in range (0, N):
    Vg[i] = data[i*2]
    Id[i] = data[i*2 + 1]

#
# Normalize data: subtract Id(max) current.
#
for i in range (0, N):
    Id[i] -= Id[N-1]

print "Input:"
print Vg
print Id

#
# Least squares method.
# Compute A and B coefficients for equation y = A*x + B.
#
def least_squares(x, y):
    K = len(x)
    sum_x = np.sum(x)
    sum_y = np.sum(y)
    sum_xy = np.sum(np.multiply(x, y))
    sum_x2 = np.sum(np.multiply(x, x))
    #print "Sum(Vg) =", sum_x
    #print "Sum(Id) =", sum_y
    #print "Sum(Vg*Id) =", sum_xy
    #print "Sum(Vg^2) =", sum_x2

    a = (K * sum_xy - sum_x * sum_y) / (K * sum_x2 - sum_x * sum_x)
    b = (sum_y - a * sum_x) / K
    #print "Id =", a, "* Vg +", b

    return a, b

#
# Obtain Idss and Yfs by the least squares method.
# Use first five samples to find a linear approximation:
#
(Yfs, Idss) = least_squares(Vg[:5], Id[:5])

#
# Obtain first approximation of cutoff voltage by the least squares method.
# Use five last nonzero samples and a least squares method.
#
# Id = a * Vg + b = 0
# Voff = -b/a
#
(a, b) = least_squares(Vg[-8:-3], Id[-8:-3])
Voff = -b / a

#print "a =", a, "mA/V"
#print "b =", b, "mA"

print "\nResults:"
print "    Idss =", round(Idss, 2), "mA"
print "Vds(off) =", round(Voff, 2), "V"
print "     Yfs =", round(Yfs, 2), "mA/V"

#
# Compute Vsat from formula:
# Idss = Yfs * (-Voff - Vsat/2)
# -Voff - Vsat/2 = Idss/Yfs
# Vsat/2 = -Voff - Idss/Yfs
#
Vsat = 2 * (-Voff - Idss/Yfs)
print "    Vsat =", round(Vsat, 2), "V"
