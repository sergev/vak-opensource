Measure efficiency of std::set and std::map containers.

 * Create 2 million data records
 * Store them in container, either std::set or std::map
 * Measure CPU time
 * Measure resident program size

See results below.

Rate in records per millisecond:

    Processor, Computer                         std::set        std::map
    --------------------------------------------------------------------
    Apple M2 Max,                               3856.1          2997.5
    Mac14,6
    --------------------------------------------------------------------
    Intel i9-9900K (16) @ 3.60GHz               2302.4          1953.3
    iMac19,1
    --------------------------------------------------------------------
    Allwinner sun7i (A20) Family (2) @ 960MHz   149.7           141.3
    Olimex A20-OLinuXino-LIME2
    --------------------------------------------------------------------
    Cortex-A76 (4) @ 2.40 GHz                   775.7           788.1
    Raspberry Pi 5 Model B Rev 1.0

Resident memory per record, in bytes:

    Processor, Computer                         std::set        std::map
    --------------------------------------------------------------------
    Apple M2 Max                                32.7            48.7
    Mac14,6
    --------------------------------------------------------------------
    Intel i9-9900K (16) @ 3.60GHz               32.4            48.5
    iMac19,1
    --------------------------------------------------------------------
    Allwinner sun7i (A20) Family (2) @ 960MHz   25.2            33.1
    Olimex A20-OLinuXino-LIME2
    --------------------------------------------------------------------
    Cortex-A76 (4) @ 2.40 GHz                   49.6            49.6
    Raspberry Pi 5 Model B Rev 1.0
