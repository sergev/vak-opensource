From zebub.msk.su!kiae!kiae!rd.relcom.msk.su!sequent.kiae.su!vak Wed Oct  6 01:12:17 1993
Received: from kiae.UUCP by zebub.msk.su with UUCP id AAA00151;
  (8.3/vak/1.6) Wed, 6 Oct 1993 00:56:33 +0300
Received: by sequent.kiae.su; Tue,  5 Oct 93 19:49:18 +0300
Received: from rd.relcom.msk.su by sequent.kiae.su with SMTP id AA02408
  (5.65.kiae-1  for <vak@zebub.msk.su>); Tue, 5 Oct 1993 20:46:02 +0400
Received: by rd.relcom.msk.su id AA13618
  (5.65c/vak/1.5) for vak@zebub.msk.su; Tue, 5 Oct 1993 18:46:45 +0300
Date: Tue, 5 Oct 1993 18:46:45 +0300
From: vak@rd.relcom.msk.su (Serge Vakulenko)
Message-Id: <199310051546.AA13618@rd.relcom.msk.su>
To: vak@zebub.msk.su
Subject: (fwd) rfi
Newsgroups: comp.lang.perl,comp.mail.mh,alt.sources.d
Status: RO

Organization: Relcom R&D

Xref: rdrel comp.lang.perl:4878 comp.mail.mh:838 alt.sources:1333
Newsgroups: comp.lang.perl,comp.mail.mh,alt.sources
Path: rdrel!kiae!relcom!news.eunet.fi!news.funet.fi!sunic!pipex!doc.ic.ac.uk!agate!boulder!wraeththu.cs.colorado.edu!tchrist
From: Tom Christiansen <tchrist@cs.Colorado.EDU>
Subject: rfi
Message-ID: <CE9xqu.s8@Colorado.EDU>
Followup-To: comp.lang.perl,comp.mail.mh,alt.sources.d
Summary: refile MH messages by person and/or oganization
Originator: tchrist@wraeththu.cs.colorado.edu
Sender: news@Colorado.EDU (USENET News System)
Organization: University of Colorado, Boulder
Date: Sat, 2 Oct 1993 14:32:54 GMT
Lines: 252


Archive-name: rfi

Here's a program that goes through a mail folder and refiles
messages into folders of names derived from the header lines.
It tries hard to determine the organization name, so something
from foocorp.com goes into foocorp, and a great deal more.
It goes to a lot of trouble to deal with funny hostnames
and paths.

usage: rfi [-tnscq] [-d dir] [-m file] [-u login] [msg ...] [+folder]
    -t          process To: lines as well
    -c          process Cc: lines as well
    -s          print scan the message before refiling
    -n          don't really do the refile
    -q          don't even print where you're refiling to
    -o dir      prepend directory for refiling orgs into, 
                eg. -o Org => +Org/convex
    -f dir      prepend directory for refiling from-lines into, 
                eg. -f Who => +Who/tchrist
    -r rcfile   read table of org mappings and possible remap function
    -u user     regexp specify who "you" are, default tchrist
                addresses matching your login won't be refiled against
    msg ...     defaults to cur
    +folder     source folder


For example,

    rfi -f Who -o Org all +inbox 

The -s is nice but slows it down a bunch.  I'd certainly use the -n 
to test it first.

Sometimes no amount of munging is good enough. In `mhpath +`/rfi.rc
is a file with two tables:

    %orgmap = (
	uu,     uunet,
	ox,     oxford,
	oxcnvx, oxford,
	mot,    motorola,
    );

    sub remap {
	s/^cc\@(.*)/charisse\@$1/;
	s/^dvk\@.*/dvk@usenix.com/;
    }


The %orgmap says that if the org appears to be the key, it's really the 
value.  

The &remap function applies those subsitutions to the address.  It does
this after trying to demunge the address as well as it can.


#!/usr/bin/perl
# rfi -- tchrist@cs.colorado.edu
# refile MH messages by person and/or oganization


######################

$MYLOGIN = $ENV{LOGNAME}
        || $ENV{USER}
        || (getpwuid($>))[0];

chop($MHPATH = `mhpath +`);

$MAP = "$MHPATH/rfi.rc";

require 'getopts.pl';

$oops = <<EOF;
    NOTE:  Must specify either -f or -o or both.  Use a 
	   bare dot (-f. or -o.) if you want things refiled into 
	   +from or +org.
EOF

&Getopts('u:tnsqo:m:f:') || &usage("Bad option");

$opt_f || $opt_o || &usage ("Need either -f or -o options!");

sub usage {
    warn "$0: @_\n\n" if @_;
die <<EOF;
usage: $0 [-tnscq] [-d dir] [-m file] [-u login] [msg ...] [+folder]
    -t		process To: lines as well
    -c		process Cc: lines as well
    -s		print scan the message before refiling
    -n		don't really do the refile
    -q		don't even print where you're refiling to
    -o dir	prepend directory for refiling orgs into, 
		eg. -o Org => +Org/convex
    -f dir	prepend directory for refiling from-lines into, 
		eg. -f Who => +Who/tchrist
    -r rcfile   read table of org mappings and possible remap function
    -u user	regexp specify who "you" are, default $MYLOGIN
		addresses matching your login won't be refiled against
    msg	...	defaults to cur
    +folder	source folder

    $oops

EOF
}

$MYLOGIN = $opt_u 	if $opt_u;
$MYLOGIN =~ s:^/(.*)/?$:$1:;		# people get confused

eval "/$MYLOGIN/";
if ($@) {
    $@ =~ s/at \(eval\).*\n//;
    die "$0: bad regular expression for -u option: $@\n";
} 

$MAP     = $opt_m 	if $opt_m;

$status = 0;

@ARGV = ('cur') unless @ARGV;
@files = `mhpath @ARGV`;
exit 1 if $?;
($folder) = "@ARGV" =~ /\s(\+\S+)/;

if ($opt_m || -e $MAP) {
    $! = $@ = '';
    do $MAP;
    if ($!) { die "$0: cannot do $MAP: $!\n"; } 
    if ($@) { die $@; }
} 

$/ = '';
$* = 1;

for (@files) {
    %useen = @users = %cseen = @companies = ();

    ($msg) = /(\d+)$/;

    if (!open (MSG, $_)) {
	warn "$0: cannot open $_: $!\n";
	next;
    } 

    $_ = <MSG>;
    s/\n\s+/ /g;

    $from = join (', ', /^From:\s*(.+)/ig);
    $to = ($opt_t || $from =~ /$MYLOGIN/oi) ? join (', ', /^To:\s*(.+)/ig) : '';
    $cc = $opt_c ? join (', ', /^B?Cc:\s*(.+)/ig) : '';
    $addrs = $from;
    $addrs .= ", $to" if $to;
    $addrs .= ", $cc" if $cc;

    for (split(/,\s*/, $addrs)) {
	$usa = $company = $name = $comment = $fullname = $user = $host = '';

	$from = $_;

	# addr (Full Name)
	s/\(([^)]*)\)// 		&& ($comment = $1);   

	# Full Name <addr> 
	s/(\S.*\S)?\s*<([^>]*)>/$2/  	&& ($fullname = $1);   

	# save copy of addr w/o the name or comment information
	$addr = $_;  		

	# route!user@forwarder, trim forwarder
	s/(.*!.*)\@.*/$1/;		      

	# host%user@forwarder, trim forwarder
	s/(.*%.*)\@.*/$1/;		      

	# user%host -> user@host
	s/%/\@/;			      

	# route!host!user -> user@host
	s/([^!]+!)*([^!]+)!([^!]+)$/$3\@$2/;  

	# user -> user@localhsot
	$_ .= '@localhost' unless /\@/;

	$name = $fullname || $comment;
	$name =~ s/"//g;

	s/\s+$//;

	&remap if defined &remap;

	# european academic or commerical...
	s/\.(co|ac)\././i;		

	# levels.unisa.edu.au -> levels.unisa.edu
	s/\.au$//i;

	# well.sf.ca.us -> well.sf.ca
	s/\.us$//i && $usa++;

	($user, $host) = split(/\@/);

	next if $user =~ /$MYLOGIN/oi;

	@parts = split(/\./, $host);

	pop(@parts) if 	@parts > 2 		# wish it were @tarts
                    &&  $parts[$#parts] =~ /^ca$/i
                    &&  length($parts[$#parts-1]) == 2
		    &&  !$usa;  # really only for canada, not california

	$company = $parts[$#parts - 1] || $host;

	$company =~ tr/A-Z/a-z/;
	$user    =~ tr/A-Z/a-z/;

	$company = $orgmap{$company} if $orgmap{$company};

	$company = "$opt_o/$company" if $opt_o && $opt_o ne '.';

	$user = "$opt_f/$user" if $opt_f && $opt_f ne '.';

	$opt_o && push(@companies, $company) unless $cseen{$company}++;
	$opt_f && push(@users, $user)        unless $useen{$user}++;

    } 
    if ($opt_s) {
	$scan = "scan $msg";
	$scan .= " $folder" if $folder;
	fork ? wait : exec $scan;
    }
    unless (@companies || @users) {
	warn "No place to refile $msg\n";
	next;
    } 

    $refile = "refile $msg";
    $refile .= " -src $folder"  if $folder;
    for (@companies) { $refile .= " +$_"; } 
    for (@users) { $refile .= " +$_"; } 

    print "$refile\n" 		unless $opt_d;
    next 			if     $opt_n;
    fork ? wait : exec $refile;
    $status = 1 if $?;
} 
exit ($status);
-- 
    Tom Christiansen      tchrist@cs.colorado.edu       
		    Consultant
	Boulder Colorado  303-444-3212

