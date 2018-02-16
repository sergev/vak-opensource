#! /usr/bin/octave -qf
1;

#
# N JFET transistor.
#
# Id (Vg) = 0                               for Vg <= V0
#           Yfs * (Vg - V0)^2 / (2 * Vsat)  for Vg > V0 and Vg < V0 + Vsat
#           Yfs * (Vg - V0 - Vsat/2)        for Vg >= V0 + Vsat
#
function Id = n_jfet (Vg)
    V0 = -0.4;
    Vsat = 0.3;
    I0 = 3.5;
    beta = I0 / Vsat / (-V0 - V0 - Vsat);
    v = Vg - V0;
    if (v < 0)
        Id = 0;
    elseif (v < Vsat)
        Id = beta * v * v;
    else
        Id = beta * Vsat * (v + v - Vsat);
    endif
endfunction

function result = n_jfet_vector (Vg)
    result = arrayfun (@n_jfet, Vg);
endfunction

fplot(@n_jfet_vector, [-0.5, 0]);

drawnow();
ans = input("Press Enter...");
