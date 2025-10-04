#!/usr/bin/env python3
from svgutils.compose import *

#
# Merge three layers.
#
Figure("1024pt", "1536pt",          # Canvas size; match to your largest SVG or desired output
        SVG('beige-layer.svg'),     # Bottom layer
        SVG('red-layer.svg'),       # Middle layer
        SVG('black-layer.svg'),     # Top layer
    ).save("kozak.svg")
