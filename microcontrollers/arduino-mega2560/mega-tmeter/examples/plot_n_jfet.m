#! /usr/bin/octave -qf
1;

#
# N JFET transistor.
#
# Id (Vg) = 0                               for Vg <= Voff
#           Yfs * (Vg - Voff)^2 / (2 * Vsat)  for Vg > Voff and Vg < Voff + Vsat
#           Yfs * (Vg - Voff - Vsat/2)        for Vg >= Voff + Vsat
#
function Id = n_jfet (Vg)
    Voff = -0.36;
    Idss = 3.43;
    Yfs = 16.21;

    Vsat = 2 * (-Voff - Idss/Yfs);
    v = Vg - Voff;
    if (v < 0)
        Id = 0;
    elseif (v < Vsat)
        Id = Yfs * v * v / (2 * Vsat);
    else
        Id = Yfs * (v - Vsat/2);
    endif
endfunction

function result = n_jfet_vector (Vg)
    result = arrayfun (@n_jfet, Vg);
endfunction

fplot(@n_jfet_vector, [-0.5, 0]);

drawnow();
ans = input("Press Enter...");
