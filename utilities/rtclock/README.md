# rtcclock.exe - MM58167 RTC Clock Utility for MS-DOS

A command-line utility to read and write time/date from/to the National Semiconductor MM58167 Real Time Clock chip on MS-DOS systems.

## Description

`rtcclock.exe` provides bidirectional communication with the MM58167 RTC hardware chip, allowing you to synchronize the DOS system time/date with the hardware clock. The implementation is based on the NetBSD MM58167 driver code, adapted for Turbo C++ 3.0 on MS-DOS.

For detailed hardware specifications, see [MM58167B.md](MM58167B.md).

## Hardware Requirements

- MM58167 Real Time Clock chip
- I/O port range 0x2C0-0x2DF (chip registers are mapped directly to these ports)
- MS-DOS with Turbo C++ 3.0 runtime

## Building

Compile using the provided Makefile:

```bash
make
```

This will produce `rtcclock.exe` using Turbo C++ 3.0 compiler.

## Usage

The utility supports two operations:

### Get Command - Read RTC and Set DOS Time/Date

Reads the current time and date from the RTC chip and sets the DOS system time/date. This is typically used in `AUTOEXEC.BAT` when DOS starts:

```batch
rtcclock get 2024
```

**Arguments:**
- `YYYY` - The current year (required). Must be between 1980 and 2099.

**Note:** The MM58167 chip does not store the year, so it must be provided manually. The utility uses the year you specify along with the month/day from the RTC to set the DOS date.

**Example:**
```batch
C:\> rtcclock get 2024
Set DOS time/date from RTC: 2024-12-15 14:30:45
```

### Set Command - Write DOS Time/Date to RTC

Reads the current DOS system time/date and writes it to the RTC chip. This should be performed after manually setting the DOS time/date using the `TIME` or `DATE` commands:

```batch
DATE 12-15-2024
TIME 14:30:45
rtcclock set
```

**Example:**
```batch
C:\> DATE 12-15-2024
C:\> TIME 14:30:45
C:\> rtcclock set
Set RTC time/date from DOS: 2024-12-15 14:30:45
```

## I/O Port Mapping

The MM58167 chip registers are accessed directly via I/O ports in the range 0x2C0-0x2DF. The register address (A4-A0 bits from Table III) forms the lower 5 bits of the I/O port address:

| Register | Address (A4-A0) | I/O Port | Function |
|----------|----------------|----------|----------|
| MM58167_MSEC | 0x00 | 0x2C0 | Milliseconds |
| MM58167_CSEC | 0x01 | 0x2C1 | Hundredths/Tenths of Seconds |
| MM58167_SEC | 0x02 | 0x2C2 | Seconds |
| MM58167_MIN | 0x03 | 0x2C3 | Minutes |
| MM58167_HOUR | 0x04 | 0x2C4 | Hours |
| MM58167_WDAY | 0x05 | 0x2C5 | Day of Week |
| MM58167_DAY | 0x06 | 0x2C6 | Day of Month |
| MM58167_MON | 0x07 | 0x2C7 | Month |
| MM58167_STATUS | 0x14 | 0x2D4 | Status Bit |
| MM58167_GO | 0x15 | 0x2D5 | GO Command |

## Implementation Details

### BCD Conversion

The MM58167 chip stores all time/date values in Binary Coded Decimal (BCD) format. The utility automatically converts between BCD and binary formats:
- `FROMBCD(x)` - Converts BCD byte to binary integer
- `TOBCD(x)` - Converts binary integer to BCD byte

### Coherent Reading

The chip is a ripple counter, which means counter increments can propagate while reading. To ensure a coherent read, the utility:
1. Resets the status bit by reading it
2. Reads all time/date registers
3. Checks the status bit again
4. Retries if the status indicates a counter increment occurred during the read
5. Repeats until a coherent read is achieved (or timeout after 100 retries)

### GO Command

When writing to the RTC, the utility issues a GO command (0xFF to register 0x15) before writing time values. This resets all counters less significant than minutes to zero, ensuring a clean state before loading new values.

### Bit Masking

The utility properly masks register values according to Table I in MM58167B.md:
- Month: bits D4-D0 (mask 0x1F, max value 1)
- Day: bits D5-D0 (mask 0x3F, max value 3)
- Hour: bits D5-D0 (mask 0x3F, max value 2)
- Minute: bits D6-D0 (mask 0x7F, max value 5)
- Second: bits D6-D0 (mask 0x7F, max value 5)

## DOS API Functions Used

The program uses standard DOS low-level routines from `<dos.h>`:

- `inportb(int port)` - Read byte from hardware register
- `outportb(int port, unsigned char byte)` - Write byte to hardware register
- `getdate(struct date *p)` - Get current DOS date
- `setdate(struct date *p)` - Set DOS date
- `gettime(struct time *p)` - Get current DOS time
- `settime(struct time *p)` - Set DOS time

## Error Handling

The utility performs validation and reports errors:

- Invalid command or missing arguments
- Year out of valid range (1980-2099)
- RTC read timeout (unable to get coherent read)
- Invalid date/time values from RTC
- DOS API errors (unable to set/get system time/date)

On error, the program returns exit code 1 and prints an error message to stderr.

## Limitations

1. **No Year Storage**: The MM58167 chip does not store the year, so it must be provided manually when reading from the RTC.

2. **No Leap Year Support**: The chip does not handle leap years or February 29th. Time synchronization should be maintained via NTP or manual correction if the system is powered down across a leap day.

3. **Hundredths Precision**: The RTC provides hundredths/tenths of seconds, but DOS time only stores whole seconds. Hundredths are set to 0 when reading from RTC.

4. **Day of Week**: The RTC stores day of week, but DOS calculates it automatically, so the RTC day-of-week value is not used.

## Example AUTOEXEC.BAT Integration

```batch
@echo off
REM Set DOS time/date from RTC on boot
rtcclock get 2024
```

## License

Based on NetBSD mm58167 driver code (Copyright (c) 2001 The NetBSD Foundation, Inc.), adapted for MS-DOS.
