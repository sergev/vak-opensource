#!/usr/bin/perl -w
#
#	Program to display key information about a boot ROM
#
#	GPL, Ken Yap 2001
#

use bytes;

sub getid ($)
{
	my ($offset) = @_;

	return ''  if ($offset == 0 or $offset > $len);
	my ($string) = unpack('Z32', substr($data, $offset, 32));
	return ($string);
}

$file = $#ARGV >= 0 ? $ARGV[0] : '-';
open(F, "$file") or die "$file: $!\n";
binmode(F);
# Handle up to 64kB ROM images
$len = read(F, $data, 64*1024);
close(F);
defined($len) or die "$file: $!\n";
substr($data, 0, 2) eq "\x55\xAA" or die "$file: Not a boot ROM image\n";
my ($codelen) = unpack('C', substr($data, 2, 1));
$codelen *= 512;
if ($codelen < $len) {
	my $pad = $len - $codelen;
	print "Image is $codelen bytes and has $pad bytes of padding following\n";
	$data = substr($data, 0, $codelen);
} elsif ($codelen > $len) {
	print "Image should be $codelen bytes but is truncated to $len bytes\n";}
($csum) = unpack('%8C*', $data);
print "ROM checksum: $csum \n";
exit(0);
