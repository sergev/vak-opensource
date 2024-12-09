Measure efficiency of std::set and std::map containers.

 * Create 2 million data records
 * Store them in container, either std::set or std::map
 * Measure CPU time
 * Measure resident program size

See results below.

Time per record, in microseconds:

    Processor, computer             std::set        std::map
    --------------------------------------------------------
    Apple M2 Max, Mac14,6           0.260           0.333

Resident memory per record, in bytes:

    Processor, computer             std::set        std::map
    --------------------------------------------------------
    Apple M2 Max, Mac14,6           32.653          48.710
