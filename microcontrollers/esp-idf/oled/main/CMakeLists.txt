cmake_minimum_required(VERSION 3.5)
idf_component_register(
    SRCS
        "draw.c"
        "font.c"
        "font_Comic_Neue_25x28.c"
        "font_Liberation_Sans_15x16.c"
        "font_Liberation_Serif_19x19.c"
        "font_Ubuntu_Mono_6x10.c"
        "iface_esp32_i2c.c"
        "iface_esp32_spi.c"
        "iface_virtual.c"
        "main.c"
        "ssd1306.c"

    INCLUDE_DIRS
        ""
)
