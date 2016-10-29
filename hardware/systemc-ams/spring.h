/*
 * Single-dimentional model of torsion spring.
 *
 * Copyright (C) 2014 Serge Vakulenko
 */
#include <systemc-ams>

using namespace sca_tdf;

SCA_TDF_MODULE(elastic)
{
    sca_in<double> displacement_in;     // input port

    sca_out<double> force_out;          // output port

    double stiffness;                   // parameters

    elastic(sc_module_name nm,          // constructor
        double k = 1.0)                 // 1 newton/meter by default
    {
        stiffness = k;
    }

    void set_attributes()               // called at elaboration
    {
        set_timestep(0.001, SC_SEC);    // time between activations
    }

    void processing()                   // executed at each activation
    {
        double displacement = displacement_in.read();
        double force = - displacement * stiffness;

        force_out.write(force);
    }
};
