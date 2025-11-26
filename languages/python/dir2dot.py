#!/usr/bin/env python3
import os
from graphviz import Digraph

def directory_to_dot(root_dir, output_file='contents.dot'):
    dot = Digraph(comment='Directory Tree')
    dot.attr(rankdir='LR')  # Left-to-right layout for better tree view

    # Normalize root path for node naming
    root_dir = os.path.abspath(root_dir)
    root_node = root_dir.replace('/', '_').replace('\\', '_')  # Sanitize for node ID

    dot.node(root_node, os.path.basename(root_dir) or root_dir, shape='folder')

    for dirpath, dirnames, filenames in os.walk(root_dir):
        parent_node = dirpath.replace('/', '_').replace('\\', '_')
        dirnames[:] = [d for d in dirnames if d not in {
            '.git',
            '__pycache__',
            '.pytest_cache',
            'build',
        }]
        #print(dirnames)

        for dirname in dirnames:
            child_path = os.path.join(dirpath, dirname)
            child_node = child_path.replace('/', '_').replace('\\', '_')
            dot.node(child_node, dirname, shape='folder')
            dot.edge(parent_node, child_node)

        #for filename in filenames:
        #    file_path = os.path.join(dirpath, filename)
        #    file_node = file_path.replace('/', '_').replace('\\', '_')
        #    dot.node(file_node, filename, shape='note')
        #    dot.edge(parent_node, file_node)

    dot.save(output_file)
    print(f"DOT file saved to {output_file}")

# Example usage
directory_to_dot('.')
