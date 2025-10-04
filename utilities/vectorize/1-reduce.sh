#!/bin/sh
set -x
input="kozak.png"
output="kozak-4colors.png"

# Reduce to 4 colors and despeckle
magick "$input" -despeckle -colors 4 "$output"

# Show colors
magick kozak-4colors.png -format %c histogram:info: | sort -n -r

# Result:
#       1178809: (254,254,254) #FEFEFE srgb(254,254,254)
#        179431: (6,5,5) #060505 srgb(6,5,5)
#        117463: (214,91,69) #D65B45 srgb(214,91,69)
#         97161: (224,192,156) #E0C09C srgb(224,192,156)
