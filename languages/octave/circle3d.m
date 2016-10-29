## Copyright (C) 2008 Indrek Mandre <indrek(at)mare.ee>
## For more information please see http://www.mare.ee/indrek/octave/

## Compute:
##    circle 3D coordinates
##
## usage: [X, Y, Z] = circle3d(center, normal, radius)
##
## Example:
##    [X, Y, Z] = circle3d([0 0 0], [1 0 0], 1);
##

## Author: Indrek Mandre <indrek(at)mare.ee>
## Created: 2008-07-17

function [x, y, z] = circle3d(center, normal, radius)
  p = cross(normal, [1 0 0]);
  if (dot(p, p) < 0.3 )
    p = cross(normal, [0 1 0]);
    if (dot(p, p) < 0.3 )
      p = cross(normal, [0 0 1]);
    endif
  endif
  p = p ./ norm(p);
  q = cross (normal, p);
  t = (0:pi/16:2*pi)(:);
  ret = repmat(center, size(t)) + radius * cos(t) * p + radius * sin(t) * q;
  x = ret(:, 1);
  y = ret(:, 2);
  z = ret(:, 3);
endfunction

