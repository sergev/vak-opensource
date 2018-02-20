#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Draw a picture of Idss and Voff parameters for
# a set of N-JFET transistor.
#
import matplotlib.pyplot as plt
import matplotlib.patches as patches

#
# Data from measurements.
#   Idss = 1.09 ... 50
#   Voff = -5 ... -0.32
#

Idss_2n5458 = [ 3.39, 3.53, 2.89, 3.13, 3.27, 3.28, 3.05, 3.2, 3.38, 3.2 ]
Idss_bf256c = [ 14.46, 12.23, 13.26, 15.19, 14.29, 14.3, 14.4, 11.71, 12.9, 13.36, 14.61, 13.85, 13.96, 14.76, 13.5, 13.77, 14.06, 14.69, 14.77, 13.81 ]
Idss_j201   = [ 1.4, 1.8, 2.37, 1.27, 1.09, 1.35, 1.37, 1.27, 1.28, 1.93, 1.69 ]
Idss_j310   = [ 31.52, 34.05, 34.99, 37.74, 40.23, 32.67, 32.11, 42.01, 30.86, 25.87 ]
Idss_kp303i = [ 1.83, 1.99, 1.86, 2.03 ]
Idss_kp303e = [ 11.64, 11.95, 7.62, 12.69, 13.0 ]
Idss_kp303v = [ 3.16, 4.48, 2.69, 2.97, 2.93, 3.34, 2.35, 2.88, 2.62, 2.36, 3.41, 3.15, 3.0, 2.41, 4.12, 2.93, 3.75 ]

Voff_2n5458 = [ -0.36, -0.37, -0.32, -0.34, -0.34, -0.35, -0.34, -0.36, -0.36, -0.36 ]
Voff_bf256c = [ -4.42, -3.67, -4.13, -4.64, -4.41, -4.45, -4.47, -3.73, -4.06, -4.14, -4.49, -4.24, -4.4, -4.49, -4.13, -4.23, -4.27, -4.57, -4.47, -4.18 ]
Voff_j201   = [ -1.08, -1.27, -1.57, -0.96, -0.91, -1.02, -1.02, -0.99, -0.98, -1.27, -1.19 ]
Voff_j310   = [ -3.21, -3.35, -3.43, -3.58, -3.86, -3.19, -3.37, -4.11, -3.02, -2.78 ]
Voff_kp303i = [ -0.8, -0.8, -0.88, -0.92 ]
Voff_kp303e = [ -3.62, -3.66, -2.52, -3.9, -3.41 ]
Voff_kp303v = [ -1.18, -1.64, -1.23, -1.11, -1.1, -1.33, -0.99, -1.15, -1.2, -1.13, -1.52, -1.41, -1.43, -1.08, -1.82, -1.06, -1.52 ]

xmin = -10
xmax = -0.2

ymin = 0.1
ymax = 100

#
# Draw the picture.
#
ax = plt.figure().add_subplot(111)
ax.yaxis.tick_right()
ax.yaxis.set_label_position("right")

# 2N5458: Voff = -1...-7, Idss = 2...9
plt.plot(Voff_2n5458, Idss_2n5458, 'bo')
ax.add_patch(patches.Rectangle((-7, 2), 6, 7, fill=False, edgecolor="blue"))
plt.text(-7+0.2, 2+0.1, "2N5458", color="blue")

# BF256C: Voff = -0.5...-8, Idss = 11...18
plt.plot(Voff_bf256c, Idss_bf256c, 'go')
ax.add_patch(patches.Rectangle((-8, 11), 7.5, 7, fill=False, edgecolor="green"))
plt.text(-8+0.2, 11+0.5, "BF256C", color="green")

# J201: Voff = -0.3...-1.5, Idss = 0.2...1
plt.plot(Voff_j201,   Idss_j201,   'wo')
ax.add_patch(patches.Rectangle((-1.5, 0.2), 1.2, 0.8, fill=False, edgecolor="#aaaaaa"))
plt.text(-1.5+0.02, 0.2+0.06, "J201", color="#888888")

# J310: Voff = -2...-6.5, Idss = 24...60
plt.plot(Voff_j310,   Idss_j310,   'co')
ax.add_patch(patches.Rectangle((-6.5, 24), 4.5, 36, fill=False, edgecolor="#009f9f"))
plt.text(-6.5+0.2, 24+2, "J310", color="#009f9f")

# КП303И: Voff = -0.5...-2, Idss = 1.5...5
plt.plot(Voff_kp303i, Idss_kp303i, 'mo')
ax.add_patch(patches.Rectangle((-2, 1.5), 1.5, 3.5, fill=False, edgecolor="magenta"))
plt.text(-2+0.03, 1.5+0.03, u"KP303I", color="magenta")

# КП303Е: Voff = -0.3...-8, Idss = 5...20
plt.plot(Voff_kp303e, Idss_kp303e, 'yo')
ax.add_patch(patches.Rectangle((-8.1, 5.1), 7.8, 14.9, fill=False, edgecolor="#9f9f00"))
plt.text(-8.1+0.2, 5.1+0.2, u"KP303E", color="#9f9f00")

# КП303В: Voff = -1...-4, Idss = 1.5...5
plt.plot(Voff_kp303v, Idss_kp303v, 'ro')
ax.add_patch(patches.Rectangle((-4, 1.48), 2.99, 3.42, fill=False, edgecolor="red"))
plt.text(-4+0.1, 1.48+0.03, u"KP303V", color="red")

plt.title('N JFET', fontsize=16)
plt.xlabel('Gate Voltage, V')
plt.ylabel('Drain Current, mA')
plt.xscale('symlog')
plt.yscale('symlog')
plt.grid(True)
plt.xlim(xmin, xmax)
plt.ylim(ymin, ymax)

ax.set_xticks([-0.2, -0.5, -1, -2, -5, -10])
ax.set_xticklabels(["-0.2V", "-0.5V", "-1V", "-2V", "-5V", "-10V"])
ax.set_yticks([0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100])
ax.set_yticklabels(["", "0.2mA", "0.5mA", "1mA", "2mA", "5mA", "10mA", "20mA", "50mA", "100mA"])

plt.savefig("picture")
plt.show()
