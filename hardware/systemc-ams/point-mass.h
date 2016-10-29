/*
 * Single-dimentional model of a point mass.
 *
 * Copyright (C) 2014 Serge Vakulenko
 */
#include <systemc-ams>

using namespace sca_tdf;

SCA_TDF_MODULE(point_mass)
{
    sca_in<double> force_in;            // input port

    sca_out<double> velocity_out;       // output ports
    sca_out<double> position_out;

    double mass;                        // parameters
    double position;
    double velocity;
    double time_last;

    point_mass(sc_module_name nm,       // constructor
        double m = 1.0,                 // 1kg by default
        double x = 0,
        double v = 0)
    {
        mass = m;
        position = x;
        velocity = v;
        time_last = 0;
    }

    void set_attributes()               // called at elaboration
    {
        set_timestep(0.001, SC_SEC);    // time between activations
        position_out.set_delay(1);      // delay for output signal
    }

    void processing()                   // executed at each activation
    {
        double force = force_in.read();
        double time_now = get_time().to_seconds();
        double delta_t = time_now - time_last;

        velocity += delta_t * force / mass;
        position += velocity * delta_t;
        time_last = time_now;

        velocity_out.write(velocity);
        position_out.write(position);
    }
};
