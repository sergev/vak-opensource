#!/usr/bin/python
#
# Compute parameters of N-JFET transistor.
#
import math
import numpy as np

data = [
# Vg, V   Id, mA
 -0.005,  3.350,
 -0.023,  3.089,
 -0.041,  2.741,
 -0.061,  2.379,
 -0.082,  2.146,
 -0.103,  1.885,
 -0.123,  1.581,
 -0.139,  1.334,
 -0.160,  1.087,
 -0.181,  0.884,
 -0.202,  0.725,
 -0.223,  0.536,
 -0.240,  0.420,
 -0.258,  0.275,
 -0.276,  0.217,
 -0.300,  0.087,
 -0.316,  0.058,
 -0.337,  0.000,
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
(a, b) = least_squares(Vg[-6:-1], Id[-6:-1])
Voff1 = round(-b / a, 2)

#print "a =", a, "mA/V"
#print "b =", b, "mA"

#
# Compute error in quadratic approximation: y[i] = (x[i] - x0)^2 * c
#
def compute_error(x0, x, y):
    K = len(x)
    ln_y = np.log(y)
    ln_x = np.log(x - x0)
    c = np.exp(np.sum(ln_y - 2*ln_x) / K)
    err = y - (x - x0)**2 * c
    return np.sum(err**2)

#
# Enhance cutoff approximation.
#
def compute_cutoff(x0, x, y):
    #print "--- Cutoff computation:", x0
    #print "X =", x
    #print "Y =", y

    e0 = compute_error(x0, x, y)
    #print "X0 =", x0, "Err =", e0

    while 1:
        e1 = compute_error(x0 - 0.01, x, y)
        #print "X1 =", x0-0.01, "Err =", e1, e0
        if e1 > e0:
            break
        x0 -= 0.01
        e0 = e1

    #print "Result: Err =", e0
    return x0

#
# Get better Voff approximation.
#
Voff = compute_cutoff(Voff1, Vg[-8:-1], Id[-8:-1])

print "\nResults:"
print "    Idss =", round(Idss, 2), "mA"
print "Vds(off) =", round(Voff, 2), "V"
print "     Yfs =", round(Yfs, 2), "mA/V"

#
# Compute saturation voltage Vsat.
#
Vsat = 2 * (-Voff - Idss/Yfs)
print "    Vsat =", round(Vsat, 2), "V"
