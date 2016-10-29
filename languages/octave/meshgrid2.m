#! /usr/bin/octave -qf

x = -2:0.1:2;
[xx,yy] = meshgrid(x, x);
z = sin(xx.^2 - yy.^2);
mesh(x, x, z);

print ("meshgrid.png", "-S800,800");
printf ("See resulting image in file meshgrid.png\n");
