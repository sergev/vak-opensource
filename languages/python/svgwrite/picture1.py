#!/usr/bin/env python3
#
# Draw a picture in SVG format, using svgwrite package.
# For documentation, see: https://svgwrite.readthedocs.io/en/latest/index.html
#
import svgwrite

#
# Global parameters of picture.
#
param_width = 800
param_height = 600
param_base_x = 20
param_base_y = 15
param_font_family = "sans-serif"
param_font_weight = "normal"
param_font_size = 18

#
# Draw one section at position (x,y) with width w and given label.
#
def draw_section(parent, x, y, w, label, option=""):
    # Draw lines.
    if option == "dashed":
        group = svgwrite.container.Group(stroke="black",
                                         stroke_width=1,
                                         stroke_dasharray="1 2",
                                         fill="none",
                                         )
        group.add(svgwrite.path.Path(f"M {x} {y} h {w}"))
    else:
        group = svgwrite.container.Group(stroke="black",
                                         stroke_width=1,
                                         stroke_linecap="round",
                                         fill="none",
                                         )
        group.add(svgwrite.path.Path(f"M {x} {y-10} v 10 h {w} v -10"))
    parent.add(group)

    # Draw text.
    group = svgwrite.container.Group(text_anchor="middle")
    group.add(svgwrite.text.Text(label,
                                 x=[x + w/2], y=[y - 15],
                                 font_size=param_font_size,
                                 font_family=param_font_family,
                                 font_weight=param_font_weight,
                                 ))
    parent.add(group)

#
# Render the whole picture and save to the given file.
#
def render(output_filename=None):
    drawing = svgwrite.Drawing(filename=output_filename,
                               size=(param_width, param_height),
                               viewBox=f"0 0 {param_width} {param_height}",
                               )

    group = svgwrite.container.Group()
    group.translate(param_base_x, param_base_y)
    draw_section(group, 100, 100, 90, "data")
    draw_section(group, 200, 100, 90, "bss")
    draw_section(group, 300, 100, 90, "heap", "dashed")
    draw_section(group, 400, 100, 90, "stack")
    drawing.add(group)

    if output_filename is None:
        print(drawing.tostring())
    else:
        drawing.save(pretty=True)

render("picture1.svg")
