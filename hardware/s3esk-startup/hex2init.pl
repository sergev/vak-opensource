#!/usr/bin/perl -w
#
# For anyone that is interested I have created a small Perl script
# that will take a object file in hex format (you will need to use
# bin2hex to create the hex file), and create a file containing
# Xilinx INIT_xx statements. This can be included in the onchip_ram_top.v
# file between module/endmodule pair.
# --- Steve Fielding <sfielding@base2designs.com>
#
use FileHandle;
use File::stat;
use strict;
use sigtrap;
use Getopt::Std;

my %options = ();
getopts("i:o:", \%options);

my $outFile = new FileHandle;
my $outFileName = "";
my @fileAsList;
my $i;
my $myItem;

my $inputFileName;
my $BRAM7 = "";
my $BRAM6 = "";
my $BRAM5 = "";
my $BRAM4 = "";
my $BRAM3 = "";
my $BRAM2 = "";
my $BRAM1 = "";
my $BRAM0 = "";

print("--- hex2init ---\n");

# --- input file
if (defined $options{i}) {
	$inputFileName = $options{i};
	if (defined $options{o}) {
		$outFileName = $options{o};
	}
	else {
		$outFileName = $inputFileName . ".init";
	}
	$outFile->open(">$outFileName") or die("Cannot open write file!\n");
} else {
	if (defined $options{o}) {
		print("Cannot use -o option without -i option\n");
		die("ie you must have an input file before you can specify an output file\n");
	}
}

getInputFile(\@fileAsList, $inputFileName);
for($i = 0; $i < @fileAsList + 0; $i += 1) {	# for every line in hex file
	$myItem = (@fileAsList)[$i];		# get the line data
	#print ("$myItem\n");
	$myItem =~ s/\n//g;			# discard carriage return
	$myItem =~ s/\s+$//;			# discard trailing space
	$BRAM7 .= substr($myItem, 0, 1);
	$BRAM6 .= substr($myItem, 1, 1);
	$BRAM5 .= substr($myItem, 2, 1);
	$BRAM4 .= substr($myItem, 3, 1);
	$BRAM3 .= substr($myItem, 4, 1);
	$BRAM2 .= substr($myItem, 5, 1);
	$BRAM1 .= substr($myItem, 6, 1);
	$BRAM0 .= substr($myItem, 7, 1);
}
#print ("$BRAM7\n");

genInit($BRAM7, "block_ram_31");
genInit($BRAM6, "block_ram_30");
genInit($BRAM5, "block_ram_21");
genInit($BRAM4, "block_ram_20");
genInit($BRAM3, "block_ram_11");
genInit($BRAM2, "block_ram_10");
genInit($BRAM1, "block_ram_01");
genInit($BRAM0, "block_ram_00");

$outFile->close();

#------------------------------ genInit ---------------------------------------#
sub genInit
{
	my $myBRAM = shift();
	my $BRAMname = shift();

	my $initBlock;
	my $initBlockNum;
	my $initBlockRev;

	printf ("myBRAM length = 0x%0x \n", length($myBRAM));
	my $padLength = 4096 - length($myBRAM);
	printf ("myBRAM pad length = 0x%0x \n", $padLength);
	#pad the string so that we have exactly 16384 bits or 2048 bytes per BRAM
	for ($i = 0; $i < $padLength; $i += 1) {
		$myBRAM .= "0";
	}
	printf ("myBRAM length = 0x%0x \n", length($myBRAM));


	$outFile->printf("// ----------------------- %s -------------------------\n", $BRAMname);
	for ($initBlockNum = 0; $initBlockNum < 64; $initBlockNum += 1) {
		#print ("initBlockNum = $initBlockNum\n");
		$initBlock = substr($myBRAM, ($initBlockNum * 64), 64);
		$initBlockRev = "";
		for ($i = 63; $i >= 0; $i = $i -1) {
			$initBlockRev .= substr ($initBlock, $i, 1);
		}
		$outFile->printf("//synthesis attribute INIT_%02X %s \"", $initBlockNum, $BRAMname);
		$outFile->print("$initBlockRev\"\n");
	}
	$outFile->print("\n");
}

#--------------------------- getInputFile -------------------------------------#
sub getInputFile
{
	my $inList = shift();
	my $inputFileName = shift();
	my $myFileAsString;
	my $remainder;
	my $i;

	$/ = undef;
	#check to see if input file exists
	if (!(-e $inputFileName) || !(-r $inputFileName)) {
		print("ERROR: Cannot open $inputFileName for reading!\n");
		die "Exiting fpgaConfig\n";
	}
	print("Reading \"$inputFileName\"...\n");
	$myFileAsString = slurpFile($inputFileName);
	printf("\"$inputFileName\" size is 0x%0x bytes\n", length($myFileAsString));
	@$inList = split /\n/, $myFileAsString;
}

#----------------------------- slurpFile --------------------------------------#
sub slurpFile
{
	my $fileAsString;

	my $filename = shift();
	my $read	= new FileHandle;	# The input file
	$read->open($filename) or print("Cannot open $filename for reading!\n") and return(0);
	$fileAsString = $read->getline();	# Read in the entire file as a string
	$read->close() or die("Cannot close $_!\n");
	return $fileAsString;
}
