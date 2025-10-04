#!/usr/bin/env python3
from lxml import etree

tree = etree.parse("kozak.svg")
root = tree.getroot()

# Remove all existing metadata.
for m in tree.xpath('//*[local-name()="metadata"]'):
    parent = m.getparent()
    if parent is not None:
        parent.remove(m)

# Add copyright.
metadata = etree.Element("metadata")
metadata.text = "(c) 2025 Serge Vakulenko. Licensed under CC BY-SA 4.0."
root.insert(0, metadata)

tree.write("kozak.svg", xml_declaration=True, encoding="ascii", standalone=True)
