#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Перекодировка из семибитного кода КОИ-7 Н2
# (коды дисплея Videoton-340) в кодировку UTF-8.
# Copyright (C) 2016 Serge Vakulenko <vak@cronyx.ru>
#
import sys

if len(sys.argv) != 2:
    print "Usage: koi7-to-utf8 file"
    sys.exit (1)

translate = {
    '`':'Ю', 'a':'А', 'b':'Б', 'c':'Ц', 'd':'Д', 'e':'Е', 'f':'Ф', 'g':'Г',
    'h':'Х', 'i':'И', 'j':'Й', 'k':'К', 'l':'Л', 'm':'М', 'n':'Н', 'o':'О',
    'p':'П', 'q':'Я', 'r':'Р', 's':'С', 't':'Т', 'u':'У', 'v':'Ж', 'w':'В',
    'x':'Ь', 'y':'Ы', 'z':'З', '{':'Ш', '|':'Э', '}':'Щ', '~':'Ч',
}

# Встретилась русская буква, обрабатываем
def decode_index(i):
    global body
    c = body[i]
    #print c,

    body = body[:i] + translate[c] + body[i+1:]

# Обрабатываем файл
f = open(sys.argv[1])
body = f.read().encode("utf-8")
i = 0
while i < len(body):
    next = i+1

    c = body[i]
    if c >= '`' and c <= '~':
        decode_index(i)

    i = next

sys.stdout.write(body)
