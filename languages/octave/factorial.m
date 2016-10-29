#! /usr/bin/octave -qf

## Copyright (C) 2005 Barry O'Donovan
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public
## License as published by the Free Software Foundation;
## either version 2, or (at your option) any later version.
##
## Octave is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied
## warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
## PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public
## License along with Octave; see the file COPYING.  If not,
## write to the Free Software Foundation, 59 Temple Place -
## Suite 330, Boston, MA 02111-1307, USA.

## usage: answer = lg_factorial(n)
##
## Returns the factorial of n (n!). n should be a positive
## integer or 0.

## Author: Barry O'Donovan <barry@ihl.ucd.ie>
## Maintainer: Barry O'Donovan <barry@ihl.ucd.ie>
## Created: February 2005
## Version: 0.1
## Keywords: factorial

function answer = lg_factorial(n)

    if (nargin != 1)
        usage("factorial(n)");
    elseif (!isscalar(n) || !isreal(n))
        error("n must be a positive integer value");
    elseif (n < 0)
        error("there is no definition for negative factorials");
    endif

    if (n == 0)
        answer = 1;
        return;
    else
        answer = prod(1:n);
    endif

endfunction

lg_factorial(20)
