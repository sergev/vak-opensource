LED cube examples for Linkit Smart 7688 board.

The cube is connected to the Linkit Smart 7688 board,
pins P10-P13, P26-P31.

    Cube  Function Linkit MRAA  Function
    -----------------------------------
    P1/1   +5V     5V     ---
    P1/2   Y0      P10    gpio2    \
    P1/3   Y1      P11    gpio3    | Layer select
    P1/4   Y2      P13    gpio1    /
    P1/5   Y3      P25    gpio6    - Upper backlight
    P1/6   Y4      P26    gpio18   - Lower backlight
    P1/7   SDI     P27    gpio19   - Serial data   \
    P1/8   CLK     P28    gpio17   - Clock         | to shift registers
    P1/9   /LE     P29    gpio16   - Latch enable  |
    P1/10  /OE     P30    gpio15   - Output enable /
    P1/11  EXT     P31    gpio14   - Unknown
    P1/12  GND     GND    ---
    ---    Sensor  P21    gpio4    - Sensor input


            Linkit                           Cube
        |              |                       ------
        |           P21|-- sensor              |
        |              |                     --|GND
        |              |                     --|EXT
  gnd --|GND           |                     --|/OE
        |           P25|-- y3                --|/LE
        |           P26|-- y4                --|CLK
  +5v --|5V         P27|-- sdi               --|SDI
   y0 --|P10        P28|-- clk               --|Y4
   y1 --|P11        P29|-- /le               --|Y3
        |           P30|-- /oe               --|Y2
   y2 --|P13        P31|-- ext               --|Y1
        ----------------                     --|Y0
                                             --|+5V
                                               |
                                               -------
