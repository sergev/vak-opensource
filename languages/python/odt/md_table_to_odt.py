#!/usr/bin/env python3
"""
md_table_to_odt.py
Reads a Markdown file (or stdin) containing a pipe-style table and writes output.odt.

Usage:
    python md_table_to_odt.py input.md output.odt
or
    cat input.md | python md_table_to_odt.py - output.odt

Requires: odfpy
    pip install odfpy
"""

import sys
import re

# friendly import check
try:
    from odf.opendocument import OpenDocumentText
    from odf.style import Style, TextProperties, ParagraphProperties, TableCellProperties
    from odf.table import Table, TableRow, TableCell, TableColumn
    from odf.text import P
except Exception:
    print("Error importing odfpy. Install it with: python3 -m pip install odfpy")
    raise

# ---------- Markdown table parsing ----------
def parse_markdown_table(lines):
    """
    Find the first pipe-style markdown table in lines and parse it.
    Returns (rows: list[list[str]], aligns: list[str]) or (None, None) if no table found.

    aligns elements are 'left'|'center'|'right' (determined from separator row).
    """
    i = 0
    n = len(lines)
    table_block = []
    while i < n:
        line = lines[i].rstrip("\n")
        # identify a candidate header line (contains '|' and at least one letter/number)
        if "|" in line and re.search(r"[^\s\|\-:]", line):
            # look ahead for separator row
            if i+1 < n and re.match(r'^\s*\|?[\s:-]+\|?[\s\|\-:]*$', lines[i+1]):
                # collect table lines until blank or non-pipe-like
                j = i
                while j < n and ("|" in lines[j] or re.match(r'^\s*$', lines[j])):
                    table_block.append(lines[j].rstrip("\n"))
                    j += 1
                break
        i += 1

    if not table_block:
        return None, None

    # normalize: remove leading/trailing pipes and split by '|', then strip cells
    def split_row(s):
        s = s.strip()
        # remove leading/trailing pipe
        if s.startswith("|"): s = s[1:]
        if s.endswith("|"): s = s[:-1]
        parts = [c.strip() for c in s.split("|")]
        return parts

    header = split_row(table_block[0])
    sep = split_row(table_block[1]) if len(table_block) > 1 else None
    if sep is None:
        return None, None

    # determine alignment from sep tokens
    aligns = []
    for token in sep:
        t = token.strip()
        if t.startswith(":") and t.endswith(":"):
            aligns.append("center")
        elif t.endswith(":"):
            aligns.append("right")
        elif t.startswith(":"):
            aligns.append("left")
        else:
            aligns.append("left")

    rows = [header]
    for r in table_block[2:]:
        if r.strip() == "":
            continue
        row = split_row(r)
        # if row has fewer cells than header, pad
        if len(row) < len(header):
            row += [""] * (len(header) - len(row))
        rows.append(row)
    # ensure all rows have equal columns (pad)
    maxc = max(len(r) for r in rows)
    for r in rows:
        if len(r) < maxc:
            r += [""] * (maxc - len(r))
    # alignments pad
    if len(aligns) < maxc:
        aligns += ["left"] * (maxc - len(aligns))
    return rows, aligns

# ---------- ODT writing ----------
def create_odt_table(rows, aligns, out_filename):
    doc = OpenDocumentText()

    # paragraph styles for alignments (added to doc.styles)
    p_left = Style(name="PLeft", family="paragraph")
    p_left.addElement(ParagraphProperties(textalign="start"))
    doc.styles.addElement(p_left)

    p_center = Style(name="PCenter", family="paragraph")
    p_center.addElement(ParagraphProperties(textalign="center"))
    doc.styles.addElement(p_center)

    p_right = Style(name="PRight", family="paragraph")
    p_right.addElement(ParagraphProperties(textalign="end"))
    doc.styles.addElement(p_right)

    # header text style (bold, centered) - automatic style
    p_header = Style(name="PHeader", family="paragraph")
    p_header.addElement(TextProperties(fontweight="bold"))
    p_header.addElement(ParagraphProperties(textalign="center"))
    doc.automaticstyles.addElement(p_header)

    # table cell styles (automatic styles)
    header_cell_style = Style(name="HeaderCell", family="table-cell")
    header_cell_style.addElement(TableCellProperties(backgroundcolor="#f2f2f8", padding="0.1cm"))
    doc.automaticstyles.addElement(header_cell_style)

    odd_cell_style = Style(name="OddCell", family="table-cell")
    odd_cell_style.addElement(TableCellProperties(backgroundcolor="#ffffff", padding="0.08cm"))
    doc.automaticstyles.addElement(odd_cell_style)

    even_cell_style = Style(name="EvenCell", family="table-cell")
    even_cell_style.addElement(TableCellProperties(backgroundcolor="#fbfbfb", padding="0.08cm"))
    doc.automaticstyles.addElement(even_cell_style)

    # create the table
    table = Table(name="TableFromMarkdown")

    # IMPORTANT FIX:
    # add one TableColumn element per column so the renderer allocates visible columns.
    num_cols = len(rows[0]) if rows else 0
    for _ in range(num_cols):
        # If you want fixed widths, you can pass e.g. TableColumn(columnwidth="5cm")
        # but attribute names vary slightly across odfpy versions; plain TableColumn() is safe.
        table.addElement(TableColumn())

    for row_idx, row in enumerate(rows):
        tro = TableRow()
        is_header = (row_idx == 0)
        for col_idx, cell_text in enumerate(row):
            # choose style by reference (odfpy allows passing style objects)
            if is_header:
                cell_style = header_cell_style
            else:
                cell_style = even_cell_style if (row_idx % 2 == 0) else odd_cell_style

            tc = TableCell(stylename=cell_style)
            # choose paragraph style for alignment
            align = aligns[col_idx] if col_idx < len(aligns) else "left"
            if is_header:
                p = P(stylename=p_header, text=cell_text)
            else:
                if align == "center":
                    p = P(stylename=p_center, text=cell_text)
                elif align == "right":
                    p = P(stylename=p_right, text=cell_text)
                else:
                    p = P(stylename=p_left, text=cell_text)
            tc.addElement(p)
            tro.addElement(tc)
        table.addElement(tro)

    doc.text.addElement(table)
    doc.save(out_filename)
    print(f"Wrote {out_filename}")

# ---------- CLI ----------
def main():
    if len(sys.argv) != 3:
        print("Usage: python md_table_to_odt.py input.md output.odt")
        sys.exit(1)
    infile = sys.argv[1]
    outfile = sys.argv[2]
    if infile == "-":
        text = sys.stdin.read().splitlines()
    else:
        with open(infile, "r", encoding="utf-8") as f:
            text = f.readlines()

    rows, aligns = parse_markdown_table(text)
    if rows is None:
        print("No Markdown table found in input.")
        sys.exit(2)

    create_odt_table(rows, aligns, outfile)

if __name__ == "__main__":
    main()
