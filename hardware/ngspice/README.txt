How to create schematics for ngspice
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tutorial: http://wiki.geda-project.org/geda:csygas

(1) Use gschem, available on Ubuntu as a component of GNU EDA package.

        sudo apt-get install geda

(2) For transistor components, use nmos-3.sym and pmos-3.sym
    from "Basic devices" library.

(3) Set correct 'refdes' attributes for all components, like Qn or Qp.

(3) Add 'device' attribute with a name of the spice subsocket,
    for example 2N7002 or BSS84.
