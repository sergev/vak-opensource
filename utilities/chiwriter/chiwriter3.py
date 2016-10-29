#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Converter from ChiWriter3 into text in UTF8 encoding.
# Copyright (C) 2016 Serge Vakulenko <serge@vak.ru>
#
# This file is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Syntax of Chiwriter Files:
#   \+          superscript row follows
#   \-          subscript row follows
#   \=          end of text block
#   \Hx, \Fx    header/footer, x=D for default, E for even, 1..9 for page 1..9
#   \S          separator
#   \Nx         footnote text
#   \Uxname     x=1..0, !..*, name = font name
#               using font# x for the font with the given name
#   \0 ... \9   font change (1-10)
#   \! ... \*   font change (11-20)
#   \           soft space
#   \,          hard return
#   \/          no/soft page break
#   \G          graphics
#   \A          after soft hyphen
#   \\          backslash
#   \@          page number
#   \^          expanding marker (for centering)
#   \[          tab
#   \]x         reverse tab
#   \Fx         footnote
#
import sys, string

if len(sys.argv) != 2:
    print "Usage: chiwriter3 input.chi > output.txt"
    sys.exit(1)

#
# Перекодируем символ русского шрифта
#
def encode_russian(c):
    # Lowercase
    if c == 'q': return u"й"
    if c == 'w': return u"ц"
    if c == 'e': return u"у"
    if c == 'r': return u"к"
    if c == 't': return u"е"
    if c == 'y': return u"н"
    if c == 'u': return u"г"
    if c == 'i': return u"ш"
    if c == 'o': return u"щ"
    if c == 'p': return u"з"
    if c == '[': return u"х"
    if c == ']': return u"ъ"
    if c == 'a': return u"ф"
    if c == 's': return u"ы"
    if c == 'd': return u"в"
    if c == 'f': return u"а"
    if c == 'g': return u"п"
    if c == 'h': return u"р"
    if c == 'j': return u"о"
    if c == 'k': return u"л"
    if c == 'l': return u"д"
    if c == ';': return u"ж"
    if c == '\'':return u"э"
    if c == 'z': return u"я"
    if c == 'x': return u"ч"
    if c == 'c': return u"с"
    if c == 'v': return u"м"
    if c == 'b': return u"и"
    if c == 'n': return u"т"
    if c == 'm': return u"ь"
    if c == ',': return u"б"
    if c == '.': return u"ю"

    # Uppercase
    if c == 'Q': return u"Й"
    if c == 'W': return u"Ц"
    if c == 'E': return u"У"
    if c == 'R': return u"К"
    if c == 'T': return u"Е"
    if c == 'Y': return u"Н"
    if c == 'U': return u"Г"
    if c == 'I': return u"Ш"
    if c == 'O': return u"Щ"
    if c == 'P': return u"З"
    if c == '{': return u"Х"
    if c == '}': return u"Ъ"
    if c == 'A': return u"Ф"
    if c == 'S': return u"Ы"
    if c == 'D': return u"В"
    if c == 'F': return u"А"
    if c == 'G': return u"П"
    if c == 'H': return u"Р"
    if c == 'J': return u"О"
    if c == 'K': return u"Л"
    if c == 'L': return u"Д"
    if c == ':': return u"Ж"
    if c == '"': return u"Э"
    if c == 'Z': return u"Я"
    if c == 'X': return u"Ч"
    if c == 'C': return u"С"
    if c == 'V': return u"М"
    if c == 'B': return u"И"
    if c == 'N': return u"Т"
    if c == 'M': return u"Ь"
    if c == '<': return u"Б"
    if c == '>': return u"Ю"
    return c

#
# Перекодируем символ шрифта MATHI
#
def encode_math1(c):
    if c <= ' ': return " "
    if c == ':': return u"÷"
    if c == '|': return u"│"
    print "\nUnknown symbol MATHI '" + c + "'"
    return c

#
# Перекодируем символ шрифта MATHII
#
def encode_math2(c):
    if c <= ' ': return " "
    if c == '3': return u"└"
    if c == '_': return u"▔"
    if c == '-': return u"▁"
    if c == '?': return u"│"
    if c == 'r': return u"√"    # square root
    if c == '5': return u"┌"    # square root upper left
    if c == '6': return u"┐"    # square root upper right
    print "\nUnknown symbol MATHII '" + c + "'"
    return '.'

#
# Перекодируем символ шрифта LINEDRAW
#
def encode_linedraw(c):
    if c <= ' ': return c
    if c == '/': return "/"
    if c == '[': return u"╶"
    if c == ']': return u"╴"
    if c == '-': return u"─"
    if c == 'p': return u"│"
    if c == '1': return u"│"
    if c == 'u': return u"┌"
    if c == 'i': return u"┬"
    if c == 'o': return u"┐"
    if c == 'm': return u"└"
    if c == '.': return u"┘"
    if c == 'j': return u"├"
    if c == 'l': return u"┤"

    if c == '+': return u"═"
    if c == '~': return u"║"
    if c == 'Q': return u"╔"
    if c == 'E': return u"╗"
    if c == 'Z': return u"╚"
    if c == 'C': return u"╝"
    if c == 'T': return u"╤"
    if c == 't': return u"╥"

    if c == 'L': return u"→"
    if c == 'J': return u"←"
    if c == '<': return u"↓"
    if c == 'I': return u"↑"
    if c == '"': return u"⇒"
    if c == ':': return u"⇐"
    if c == '_': return u"╌"
    if c == 'P': return u"╎"
    if c == '{': return u"╶"
    if c == '}': return u"╴"

    if c == '=': return u"━"
    if c == '`': return u"┃"
    if c == 'q': return u"┏"
    if c == 'e': return u"┓"
    if c == 'z': return u"┗"
    if c == 'c': return u"┛"
    if c == 'w': return u"┳"
    if c == 'x': return u"┻"
    print "\nUnknown symbol LINEDRAW '" + c + "'"
    return c

#
# Перекодируем символ в соответствии с текущим шрифтом
#
def encode_font(c):
    global font

    if font == '0':  return encode_math2(c)     # MATHII
    if font == '1':  return c                   # STANDARD
    if font == '2':  return c                   # SMALL
    if font == '3':  return c                   # ITALIC
    if font == '4':  return c                   # BOLD
    if font == '8':  return encode_linedraw(c)  # LINEDRAW
    if font == '9':  return encode_math1(c)     # MATHI
    if font == '&':  return encode_russian(c)   # BOLDALTR
    if font == '%':  return encode_russian(c)   # RUSSIAN
    if font == '#':  return encode_russian(c)   # RUSSIAN
    if font == '\'': return encode_russian(c)   # RUSSIAN

    print "\nUnknown font \\" + font
    font = '1'
    return ""

#
# Обрабатываем строку
#
def process_line(line):
    global font

    result = ""
    backslash = 0
    for c in line:
        if backslash:
            backslash = 0
            if c == ' ':                # Space
                result += ' '
            elif c == '\\':             # Backslash
                result += '\\'
            elif c == ',':              # Newline
                result += '\n'
            elif c == '[':              # Tab, replace by space
                result += ' '
            elif c == '/':              # Page break, ignore
                result += ''
            elif c == 'B':              # Unknown, ignore
                result += ''
            elif c == 'F':              # Footnote
                result += '{footnote}'
                font = '1'
            elif c == '^':              # Marker for centering, ignore
                result = result
            elif c >= '0' and c <= '9': # Set font
                font = c
            elif c >= '!' and c <= '*': # Set font extended
                font = c
            else:
                print "\nUnknown '\\" + c + "'"
            continue

        if c == '\\':
            backslash = 1
            continue

        result += encode_font(c)

    print result.encode("utf-8")

#
# Проверяем первую строчку
#
def check_version(line):
    if line[:5] != "\\1cw ":
        print "Incorrect header:", line
        sys.exit(1)
    #print "ChiWriter", line[5:]

#
# Обрабатываем файл
#
f = open(sys.argv[1])
first_line = 1
in_footnote = 0
font = '1'
for line in f.readlines():
    # Convert from utf8 and strip trailing newline and spaces
    line = unicode(line, "utf-8")
    line = line.rstrip(" \r\n")

    if first_line:
        # First line "\1cw"
        check_version(line)
        first_line = 0
        continue

    if line[:1] == '\32':
        # ^Z - end of file
        break

    if line[:2] == "\\=":
        # \= - end of text block, ignore
        in_footnote = 0
        continue

    if line[:2] == "\\U":
        # \Uxname - using font# x for the font with the given name
        #print "Font " + line[3:] + " as \\" + line[2]
        continue

    if line[:2] == "\\p":
        # \pXX value - numeric parameter
        value = line[4:].strip()
        #print "Parameter " + line[2:4] + " = " + value
        continue

    if line[:2] == "\\N":
        # \N! - unknown, ignore
        continue

    if line[:3] == "\\FD" or line[:3] == "\\F3":
        # \FD - define footnote, ignore
        in_footnote = 1
        continue

    if line[:2] == "\\-":
        # \- subscript row
        if line[2:] == "":
            # Empty, ignore
            continue
        line = line[2:]

    if line[:2] == "\\+":
        # \- superscript row
        if line[2:] == "":
            # Empty, ignore
            continue
        line = line[2:]

    if in_footnote:
        # Ignore footnote contents
        continue

    process_line(line)
