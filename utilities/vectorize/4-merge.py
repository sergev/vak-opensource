#!/usr/bin/env python3
from svgutils.compose import *
from svgutils.transform import fromfile

width, height = fromfile('black-layer.svg').get_size()

#
# Merge three layers.
#
Figure(width, height,               # Canvas size
        SVG('beige-layer.svg'),     # Bottom layer
        SVG('red-layer.svg'),       # Middle layer
        SVG('black-layer.svg'),     # Top layer
    ).save("kozak.svg")
