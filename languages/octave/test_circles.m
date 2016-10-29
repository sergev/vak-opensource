#! /usr/bin/octave -qf
## Copyright (C) 2008 Indrek Mandre <indrek(at)mare.ee>
## For more information please see http://www.mare.ee/indrek/octave/

R = 0.15;
S = 0.08;
L = R + S / sqrt(2);

hold on
view(-15, 30);
[x, y, z] = circle3d([-L 0 0], [1 0 0], R); plot3(x, y, z);
[x, y, z] = circle3d([L 0 0], [-1 0 0], R); plot3(x, y, z);
[x, y, z] = circle3d([0 -L 0], [0 1 0], R); plot3(x, y, z);
[x, y, z] = circle3d([0 L 0], [0 -1 0], R); plot3(x, y, z);
[x, y, z] = circle3d([0 0 -L], [0 0 1], R); plot3(x, y, z);
[x, y, z] = circle3d([0 0 L], [0 0 -1], R); plot3(x, y, z);
view(-15, 30);
axis square
hold off

print ("test_circles.png", "-S800,800");
printf ("See resulting image in file test_circles.png\n");
