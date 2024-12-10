Here I will collect log files with system information from various
computers and operating systems.

For Linux:
    cat /proc/cpuinfo
    sudo dmesg
    sudo lshw
    lspci -v
    sudo lsusb -v
    neofetch --stdout

For FreeBSD:
    sudo dmesg
    sysctl -a
    lspci -v
    sudo lsusb -v
    neofetch --stdout

For MacOS:
    sysctl -a
    cpu-info
    isa-info
    cache-info
    system_profiler -detailLevel mini
    fastfetch --logo none
