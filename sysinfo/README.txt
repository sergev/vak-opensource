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
