# MCP2221 HID Library

This is a library for interfacing with the HID features of the [MCP2221 USB to UART and I2C/SMBus serial converter from Microchip](http://www.microchip.com/wwwproducts/Devices.aspx?product=MCP2221). The converter includes 4 GPIO pins, 3x 10-bit ADCs, 1x 5-bit DAC and more.

Microchip does provide a library for interfacing with the chip, however it is supplied as proprietary DLLs. This project aims to be an open-source and multi-platform alternative.

This library also makes use of [HIDAPI](http://www.signal11.us/oss/hidapi/).

| Feature | Status
| ------- | ------
| ADC     | Supported
| DAC     | Supported
| GPIO    | Supported
| Interrupt input        | Supported
| Clock reference output | Supported
| USB Descriptors (Manufacturer, product, serial, VID, PID) | Supported
| I2C/SMB | Limited support, WIP
| Flash password protection | Not yet implemented
| C++ and C# wrappers       | Not yet implemented

## Documentation
[Doxygen pages](http://zkemble.github.io/libmcp2221/)

Hint:

| Function prefix | Description
| --------------- | -----------
| mcp2221_set*    | Set SRAM config, this takes immediate effect
| mcp2221_get*    | Get SRAM config
| mcp2221_save*   | Writes to flash, this setting is used at startup
| mcp2221_load*   | Read from flash
| mcp2221_read*   | Read ADC/GPIO/interrupt values

## Setting up
### Using pre-built binaries
- Copy `libmcp2221.h` to your compilers include directory (`/usr/include/` on Linux)
- Windows: Copy `./lib/win/libmcp2221.dll` to your compilers lib directory. Each program that uses libmcp2221 will need a copy of `libmcp2221.dll` in the same directory.
- Linux: Copy `./lib/unix/libmcp2221.so` and `./lib/unix/libmcp2221.a` to `/usr/lib/` (these were compiled on Debian 8.4 x64 using the HIDRAW version of HIDAPI)

### Compiling
- Download hid.c for your OS from https://github.com/signal11/hidapi ([Windows](https://raw.githubusercontent.com/signal11/hidapi/master/windows/hid.c) | [Linux (HIDRAW)](https://raw.githubusercontent.com/signal11/hidapi/master/linux/hid.c) | [Linux (LibUSB - this doesn't seem to work at the moment)](https://raw.githubusercontent.com/signal11/hidapi/master/libusb/hid.c) | [Mac](https://raw.githubusercontent.com/signal11/hidapi/master/mac/hid.c)) and [hidapi.h](https://raw.githubusercontent.com/signal11/hidapi/master/hidapi/hidapi.h)
- Place `hid.c` and `hidapi.h` files into the libmcp2221 directory
- Linux HIDAPI requires dev packages for libudev and libusb `apt-get install libudev-dev libusb-1.0-0-dev`
- Run `make`
- Copy `libmcp2221.h` to your compilers include directory (`/usr/include/` on Linux)
- Windows: Copy `libmcp2221.dll` from the bin folder to your compilers lib directory. Each program that uses libmcp2221 will need a copy of `libmcp2221.dll` in the same directory.
- Linux: Copy `libmcp2221.so` and `libmcp2221.a` from the bin folder to `/usr/lib/`

--------

Third party contents are copyrighted by their respective authors.

My productions are published under GNU GPL v3 (see License.txt).

--------

[MCP2221 HID Library](http://blog.zakkemble.co.uk/mcp2221-hid-library/)

--------

Zak Kemble

contact@zakkemble.co.uk
