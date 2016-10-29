#! /usr/bin/octave -qf

n = 2^27;
step = 2.0 / n;
t = 0:step:2-step;

printf("Creating 1:%d data vector...\n", columns(t));
x = 2*sin(20*pi*t) + sin(100*pi*t);

printf("Computing Fourier transform...\n");
t0 = time();
y = fft(x);
t1 = time();
printf("Done.\n");

printf("Time: %d seconds\n", t1 - t0);
printf("Memory footprint: %d kbytes\n", getrusage().maxrss);
