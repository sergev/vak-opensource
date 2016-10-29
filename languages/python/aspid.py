#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# АСПИД - автоматизированная система подготовки инструкций и документации.
# Изначально была реализована на БЭСМ-6 в мониторной системе Дубна.
# Повторена на Питоне с целью обработки описания Микро-БЭСМ.
# Copyright (C) 2005 Serge Vakulenko <vak@cronyx.ru>
#
import sys, string

if len(sys.argv) != 2:
	print "Usage: aspid file"
	sys.exit (1)

#
# Игнорируем:
#	#онсц
#	#сгN
#	#нсцN
#	#дскN
#	#кр
#
# Заменяем:
#       #пч		<li>
#	#всN		<p>
#	#бсN		<p>
#	@x		большая буква X
#	&x		маленькая буква x
#	#б текст #м	ТЕКСТ большими буквами
#	#ц текст...\n	<center> текст </center>
#	#аN		<br> &nbsp; повторяем N раз
#       #новсц		<p>
#       #на		<p>
#	#перн		<li>
#
def do_command (word):
	global center_flag, upper_flag, need_cap

	# Игнорируем
	if word == u"#онсц":
		return
	if word.startswith (u"#сг") and word[3].isdecimal():
		return
	if word.startswith (u"#нсц") and word[4].isdecimal():
		return
	if word.startswith (u"#дск") and word[4].isdecimal():
		return
	if word == u"#кр":
		return

	# Заменяем
	if word == u"#пч":
		print "<li>",
	if word.startswith (u"#бс") and word[3].isdecimal():
		print "<p>"
	elif word.startswith (u"#вс") and word[3].isdecimal():
		print "<p>"
	elif word == u"#б":
		upper_flag = 1
	elif word == u"#м":
		upper_flag = 0
	elif word == u"#ц":
		center_flag = 1
		print "<center>"
	elif word.startswith (u"#а") and word[2].isdecimal():
		n = int (word[2:])
		print "<br>"
		for i in range (n):
			sys.stdout.write ("&nbsp;")
	elif word == u"#новсц":
		print "<p>",
	elif word == u"#на":
		upper_flag = 0
		need_cap = 1
		print "<p>",
	elif word == u"#перн":
		print "<li>",
	else:
		print word.encode ("utf-8"),

# Обрабатываем слово текста
def handle_word (word):
	global center_flag, upper_flag, need_cap

	if upper_flag:
		word = word.upper()
	else:
		word = word.lower()

	# Заменяем @x на большую букву X
	while 1:
		i = word.find ('@')
		if i < 0:
			break
		word = word[:i] + word[i+1].upper() + word[i+2:]

	# Заменяем &X на маленькую букву x.
	while 1:
		i = word.find ('&')
		if i < 0:
			break
		word = word[:i] + word[i+1].lower() + word[i+2:]

	# Заменяем ._б на ._Б.
	i = -1
	while 1:
		i = word.find ('._', i+1)
		if i < 0:
			break
		c = ord (word[i+2])
#		print str(i) + "***" + word.encode ("utf-8") + "***" + word[i+2].encode ("utf-8") + str(c)
		if c >= 0x401 and c <= 0x4ff:
			word = word[:i+2] + word[i+2].upper() + word[i+3:]

	if need_cap:
		word = word[0].upper() + word[1:]
		need_cap = 0

	# Заменяем подчерки на неразрывные пробелы
	word = word.encode ("utf-8")
	word = word.replace ('_', "&nbsp;")
	print word,
	if word[-1:] == '.':
		need_cap = 1

# Вставляем пробелы после точек перед буквами
def handle_dots (line):
	i = 0;
	while i < len(line) - 1:
		if line[i] == '.':
			c = ord (line[i+1])
			if (c >= 0101 and c <= 0132) or \
			   (c >= 0141 and c <= 0172) or \
			   (c >= 0x401 and c <= 0x4ff):
				line = line[:i+1] + " " + line[i+1:]
		i = i + 1
	return line

# Обрабатываем файл
f = open (sys.argv[1])
center_flag = 0
upper_flag = 0
need_cap = 1
print "<html><head>"
print "<meta HTTP-EQUIV=Content-Type CONTENT=\"text/html; charset=utf8\">"
print "<meta HTTP-EQUIV=Content-Language CONTENT=ru>"
print "</head><body>"
for line in f.readlines():
	line = unicode (line, "utf-8")
	line = handle_dots (line)

	for word in line.split():
		if word[0] == '#':
			do_command (word)
			continue
		handle_word (word)

	if center_flag:
		print "</center>",
		center_flag = 0
	print

print "</body></html>"
