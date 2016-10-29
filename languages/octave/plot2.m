#! /usr/bin/octave -qf

#x = linspace(0, 2*pi, 100);
#y = sin(x);
#figure;
#plot(x, y);
#hold on;



x = -10:.1:10;
y = sin(x).*exp(-abs(x));
plot(x,y)
grid
print ("plot.png", "-S400,400");
printf ("See resulting image in file plot.png\n");
