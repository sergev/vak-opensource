cmake_minimum_required(VERSION 3.5)
idf_component_register(
    SRCS
        "oled.c"
        "draw.c"
        "main.c"
        "lucidasans7.c"
        "lucidasans9.c"
        "lucidasans11.c"
        "lucidasans15.c"
        "lucidasans28.c"
        "verdana7.c"
        "5x7.c"
        "6x9.c"
        "digits20.c"
        "digits32.c"
        "dejavusans10.c"
        "dejavusans12.c"
        "dejavusans14.c"
        "dejavusans18.c"
        "dejavusans8.c"

    INCLUDE_DIRS
        ""
)
