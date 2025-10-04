#!/bin/sh
set -x

potrace black-layer.pbm -s --turdsize 15                   -o black-layer.svg
potrace beige-layer.pbm -s --turdsize 15 --color "#EFAC78" -o beige-layer.svg
potrace   red-layer.pbm -s --turdsize 15 --color "#C21919" -o   red-layer.svg
