#!/usr/bin/perl -w

use Getopt::Std;

use constant MINROMSIZE => 8192;
use constant MAXROMSIZE => 262144;

use strict;

use vars qw(%opts);

use bytes;

sub getromsize ($) {
	my ($romref) = @_;
	my $i;

	print STDERR "BIOS extension ROM Image did not start with 0x55 0xAA\n"
		if (substr($$romref, 0, 2) ne "\x55\xaa");
	my $size = ord(substr($$romref, 2, 1)) * 512;
	for ($i = MINROMSIZE; $i < MAXROMSIZE and $i < $size; $i *= 2) { }
	print STDERR "$size is a strange size for a boot ROM\n"
		if ($size > 0 and $i > $size);
	return ($size);
}

sub addident ($) {
	my ($romref) = @_;

	return (0) unless (my $s = $opts{'i'});
	# include the terminating NUL byte too
	$s .= "\x00";
	my $len = length($s);
	# Put the identifier in only if the space is blank
	my $pos = length($$romref) - $len - 2;
	return (0) if (substr($$romref, $pos, $len) ne ("\xFF" x $len));
	substr($$romref, $pos, $len) = $s;
	return ($pos);
}

sub writerom ($$) {
	my ($filename, $romref) = @_;

	open(R, ">$filename") or die "$filename: $!\n";
	print R $$romref;
	close(R);
}

sub checksum ($) {
	my ($romref) = @_;

	substr($$romref, 5, 1) = "\x00";
	my $sum = unpack('%8C*', $$romref);
	substr($$romref, 5, 1) = chr(256 - $sum);
	# Double check
	$sum = unpack('%8C*', $$romref);
	if ($sum != 0) {
		print "Checksum fails\n"
	} elsif ($opts{'v'}) {
		print "Checksum ok\n";
	}
}

sub makerom () {
	my ($rom, $romsize);

	getopts('3xi:p:s:v', \%opts);
	$ARGV[0] or die "Usage: $0 [-s romsize] [-i ident] [-p vendorid,deviceid] [-x] [-3] rom-file\n";
	open(R, $ARGV[0]) or die "$ARGV[0]: $!\n";
	# Read in the whole ROM in one gulp
	my $filesize = read(R, $rom, MAXROMSIZE+1);
	close(R);
	defined($filesize) and $filesize >= 3 or die "Cannot get first 3 bytes of file\n";
	print "$filesize bytes read\n" if $opts{'v'};
	# If PXE image, just fill the length field and write it out
	if ($opts{'x'}) {
		substr($rom, 2, 1) = chr((length($rom) + 511) / 512);
		&writerom($ARGV[0], \$rom);
		return;
	}
	# Size specified with -s overrides value in 3rd byte in image
	# -s 0 means round up to next 512 byte block
	if (defined($opts{'s'})) {
		if (($romsize = oct($opts{'s'})) <= 0) {
			# NB: This roundup trick only works on powers of 2
			$romsize = ($filesize + 511) & ~511
		}
	} else {
		$romsize = &getromsize(\$rom);
		# 0 put there by *loader.S means makerom should pick the size
		if ($romsize == 0) {
			# Shrink romsize down to the smallest power of two that will do
			for ($romsize = MAXROMSIZE;
				$romsize > MINROMSIZE and $romsize >= 2*$filesize;
				$romsize /= 2) { }
		}
	}
	if ($filesize > $romsize) {
		print STDERR "ROM size of $romsize not big enough for data, ";
		# NB: This roundup trick only works on powers of 2
		$romsize = ($filesize + 511) & ~511;
		print "will use $romsize instead\n"
	}
	# Pad with 0xFF to $romsize
	$rom .= "\xFF" x ($romsize - length($rom));
	if ($romsize >= 128 * 1024) {
		print "Warning: ROM size exceeds extension BIOS limit\n";
	}
	substr($rom, 2, 1) = chr(($romsize / 512) % 256);
	print "ROM size is $romsize\n" if $opts{'v'};
	my $identoffset = &addident(\$rom);
	# 3c503 requires last two bytes to be 0x80
	substr($rom, MINROMSIZE-2, 2) = "\x80\x80"
		if ($opts{'3'} and $romsize == MINROMSIZE);
	&checksum(\$rom);
	&writerom($ARGV[0], \$rom);
}

sub modrom () {
	my ($rom);

	getopts('p:v', \%opts);
	$ARGV[0] or die "Usage: $0 [-p vendorid,deviceid] rom-file\n";
	open(R, $ARGV[0]) or die "$ARGV[0]: $!\n";
	# Read in the whole ROM in one gulp
	my $filesize = read(R, $rom, MAXROMSIZE+1);
	close(R);
	defined($filesize) and $filesize >= 3 or die "Cannot get first 3 bytes of file\n";
	print "$filesize bytes read\n" if $opts{'v'};
	&checksum(\$rom);
	&writerom($ARGV[0], \$rom);
}

# Main routine. See how we were called and behave accordingly
if ($0 =~ m:modrom(\.pl)?$:) {
	&modrom();
} else {
	&makerom();
}
exit(0);
