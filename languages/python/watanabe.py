#!/usr/bin/env python3
#
# Convert picture from Watanabe plotter format into SVG format.
# Use svgwrite package.
# For documentation, see: https://svgwrite.readthedocs.io/en/latest/index.html
#
import svgwrite, sys

if len(sys.argv) != 2:
    print("Usage: watanabe.py input.txt")
    sys.exit (1)

#
# Global parameters of picture.
#
param_width = 1500
param_height = 2000

#
# Draw one section at position (x,y) with width w and given label.
#
def draw(group, multiline):
    path = "M"
    for _, item in enumerate(multiline):
        x = int(item[0])
        y = param_height - int(item[1])
        path += f" {x} {y}"

    group.add(svgwrite.path.Path(path))

#
# Render the whole picture and save to the given file.
#
def render(input_filename, output_filename=None):
    drawing = svgwrite.Drawing(filename=output_filename,
                               size=(param_width, param_height),
                               viewBox=f"0 0 {param_width} {param_height}")
    group = svgwrite.container.Group(stroke="black",
                                     stroke_width=2,
                                     stroke_linecap="round",
                                     fill="none")
    with open(input_filename) as file:
        multiline = []
        for line in file.readlines():
            if len(line) < 1:
                continue

            xy = line.strip()[1:].split(",")
            if len(xy) != 2:
                continue

            if line[0] == "M":
                if len(multiline) > 1:
                    draw(group, multiline)
                multiline = [xy]
            elif line[0] == "D":
                multiline.append(xy)
            else:
                print("Error: incorrect input: ", line.strip())
                sys.exit(1)

    drawing.add(group)

    if output_filename is None:
        print(drawing.tostring())
    else:
        drawing.save(pretty=True)

render(sys.argv[1], "watanabe.svg")
