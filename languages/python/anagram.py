#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
#
# Find anagrams.
#
import sys

if len(sys.argv) != 2:
    print("Usage: anagram.py input.txt")
    sys.exit(1)

#
# Read the dictionary.
#
dict = {}
with open(sys.argv[1]) as file:
    for line in file.readlines():
        if len(line) < 2 or line[0] == ' ' or line[0] == '#':
            continue

        word = line.split()[0]
        #print(word)
        if '-' in word:
            continue

        dict[word] = word

#
# Create a table of pairs.
#
table = {}
for word in dict:
    key = ''.join(sorted(word))
    #print("key =", key, ", word =", word)
    if key in table:
        table[key].append(word)
    else:
        table[key] = [word]
#print(table)

#
# Print the result.
#
for key in table:
    item = table[key]
    if len(item) > 1:
        print(' - '.join(item))
