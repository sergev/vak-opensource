#! /usr/bin/octave -qf

len = input("What size array do you wish to use for the evaluation: ");

clear a;
tic();
for i=1:len
    a(i) = i;
endfor
time1 = toc();

a = [1];
tic();
for i=2:len
    a = [a i];
endfor
time2 = toc();

a = zeros(len, 1);
tic();
for i=1:len
    a(i) = i;
endfor
time3 = toc();

printf("The time taken for method 1 was %.4f seconds\n", time1);
printf("The time taken for method 2 was %.4f seconds\n", time2);
printf("The time taken for method 3 was %.4f seconds\n", time3);
