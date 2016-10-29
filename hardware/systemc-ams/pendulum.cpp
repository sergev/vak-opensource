/*
 * Simulation of a pendulum.
 *
 * Copyright (C) 2014 Serge Vakulenko
 */
#include <systemc-ams.h>

#include "point-mass.h"
#include "spring.h"

int sc_main (int argc, char* argv[])
{
    // Use 1 msec time steps.
    sc_set_time_resolution(0.001, SC_SEC);

    // Declare signals.
    sca_signal<double> force;
    sca_signal<double> velocity;
    sca_signal<double> position;

    // Connect the bob.
    // Mass is 1kg, initial displacement 1 meter, zero velocity.
    point_mass bob("bob", 1.0, 1.0, 0);
    bob.force_in     (force);
    bob.velocity_out (velocity);
    bob.position_out (position);

    // Connect the spring.
    // Stiffness is 20 newtons per meter.
    elastic spring("spring", 20.0);
    spring.displacement_in (position);
    spring.force_out       (force);

    // Tracing to file:tabular or VCD format.
    using namespace sca_util;
    //sca_trace_file *tfp = sca_create_tabular_trace_file("trace");
    sca_trace_file *tfp = sca_create_vcd_trace_file("trace");
    sca_trace(tfp, force,    "force");
    sca_trace(tfp, velocity, "velocity");
    sca_trace(tfp, position, "position");

    // Simulate for ten seconds.
    sc_start(10.0, SC_SEC);

    // Terminate simulation.
    //sca_close_tabular_trace_file(tfp);
    sca_close_vcd_trace_file(tfp);
    return 0;
}
