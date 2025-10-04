#!/bin/sh
set -x

potrace black-layer.pbm -s --turdsize 15                   -o black-layer.svg
potrace beige-layer.pbm -s --turdsize 15 --color "#E7C49F" -o beige-layer.svg
potrace   red-layer.pbm -s --turdsize 15 --color "#D55A43" -o   red-layer.svg
