#!/bin/sh
# Colors:
#       1178809: (254,254,254) #FEFEFE srgb(254,254,254)
#        179431: (6,5,5) #060505 srgb(6,5,5)
#        117463: (214,91,69) #D65B45 srgb(214,91,69)
#         97161: (224,192,156) #E0C09C srgb(224,192,156)
set -x
input="kozak-4colors.png"
white="#FEFEFE"
black="#060505"
red="#D65B45"
beige="#E0C09C"

# Extract black layer
magick "$input" \
    -fill black -opaque $black \
    -fill white -opaque $white \
    -fill white -opaque $beige \
    -fill white -opaque $red \
    black-layer.pbm

# Extract red layer
magick "$input" \
    -fill black -opaque $red \
    -fill white -opaque $white \
    -fill white -opaque $black \
    -fill white -opaque $beige \
    red-layer.pbm

# Extract beige layer
magick "$input" \
    -fill black -opaque $beige \
    -fill white -opaque $white \
    -fill white -opaque $black \
    -fill white -opaque $red \
    beige-layer.pbm
