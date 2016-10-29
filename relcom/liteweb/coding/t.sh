#!/usr/local/bin/perl

while (ord ($c = getc)) {
	if (ord ($c) >= 128) {
		printf "%d", ord ($c);
		next;
	}
	print $c;
}
