#
# N JFET transistor.
#
# Id (Vg) = 0                               for Vg <= V0
#           β * (Vg - V0)^2                 for Vg > V0 and Vg < V0 + Vsat
#           β * Vsat (2 (Vg - V0) - Vsat)   for Vg >= V0 + Vsat
#
function result = n_jfet (Vg)
    result = arrayfun (jfet_current, Vg)
endfunction

V0 = -0.4;
Vsat = 0.25;
I0 = 3.5;
beta = I0 / Vsat / (-V0 - V0 - Vsat);

function Id = jfet_current (Vg)
    v = Vg - V0;
    if (v < 0)
        Id = 0;
    elseif (v < Vsat)
        Id = beta * v.^2;
    else
        Id = beta * Vsat * (2*v - Vsat);
    endif
endfunction
