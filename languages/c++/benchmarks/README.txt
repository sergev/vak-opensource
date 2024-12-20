Measure efficiency of std::set and std::map containers.

 * Create 2 million data records
 * Store them in container, either std::set or std::map
 * Measure CPU time
 * Measure resident program size

See results below. Rate is shown in records per millisecond.
Resident memory per record is in bytes.

                                            --------Rate--------    -------Memory-------
Processor, Computer                         std::set    std::map    std::set    std::map
----------------------------------------------------------------------------------------
Apple M2 Max (12) @ 3.70 GHz                3856.1      2997.5      32.7        48.7
MacBook Pro (16-inch, 2023)
----------------------------------------------------------------------------------------
Apple M2 Max (Virtual) (12)                 3568.1      2764.4      32.7        48.7
Apple Virtual Machine, MacOS
----------------------------------------------------------------------------------------
Apple M2 Max (12)                           2706.7      2169.1      33.0        49.1
Apple Virtualization Generic Platform, Chimera
----------------------------------------------------------------------------------------
Apple M1 (8) @ 3.20 GHz                     2608.3      2091.6      32.7        48.7
Mac mini (M1, 2020)
----------------------------------------------------------------------------------------
Intel Core i9-9900K (16) @ 3.60 GHz         2302.4      1953.3      32.4        48.5
iMac (Retina 5K, 27-inch, 2019)
----------------------------------------------------------------------------------------
Apple M2 Max (12)                           2155.3      2149.8      49.5        49.5
Apple Virtualization Generic Platform, Ubuntu
----------------------------------------------------------------------------------------
Intel Core i7-8750H (12) @ 4.10 GHz         1656.6      1654.2      49.8        49.8
Lenovo ThinkPad P1
----------------------------------------------------------------------------------------
Cortex-A76 (4) @ 2.40 GHz                   775.7       788.1       49.6        49.6
Raspberry Pi 5 Model B Rev 1.0
----------------------------------------------------------------------------------------
ARM Cortex-A72 r0p3 (4) @ 1.5 GHz           527.7       490.3       34.7        50.3
Raspberry Pi 4 Model B Rev 1.2
----------------------------------------------------------------------------------------
Allwinner sun7i (A20) Family (2) @ 960MHz   149.7       141.3       25.2        33.1
Olimex A20-OLinuXino-LIME2
