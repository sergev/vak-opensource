Measure efficiency of std::set and std::map containers.

 * Create 2 million data records
 * Store them in container, either std::set or std::map
 * Measure CPU time
 * Measure resident program size

See results below.

Time per record, in microseconds:

    Processor                       Computer    std::set        std::map
    --------------------------------------------------------------------
    Apple M2 Max                    Mac14,6     0.260           0.333
    Intel i9-9900K (16) @ 3.60GHz   iMac19,1    0.437           0.517

Resident memory per record, in bytes:

    Processor                       Computer    std::set        std::map
    --------------------------------------------------------------------
    Apple M2 Max                    Mac14,6     32.653          48.710
    Intel i9-9900K (16) @ 3.60GHz   iMac19,1    32.401          48.464
