#!/usr/bin/env perl
use Config;
use Sys::Hostname;
use Getopt::Long;
use File::Basename;
use File::Spec;
use Cwd;
#use LWP::Simple;

my $repo = 'http://static.saga.cct.lsu.edu/mephisto/repository';
my $tag = $ENV{'_NMI_SAGA_TAG'};

# Set up all directory paths
my $basedir = getcwd();
my $packdir = "$basedir/packages";
my $srcdir = "$basedir/sources";
print "basedir --> $basedir\n";

# Gather some information about the machine and print it out
my $host = hostname();
my $platform = $ENV{'NMI_PLATFORM'};
print "Running on $host which is a $platform platform";

# Set up the environment then print it out
sys("env | sort");

sys("mkdir $packdir");
sys("mkdir $srcdir");

chdir("$srcdir") or die " Error: Cannot change to $srcdir\n";

# Get Globus Toolkit 5.0.2
print "Getting $repo/$tag/gt5.0.2-all-source-installer.tar.bz2 to $srcdir/gt5.0.2-all-source-installer.tar.bz2\n";
#my $rc = getstore("$repo/$tag/gt5.0.2-all-source-installer.tar.bz2", "$srcdir/gt5.0.2-all-source-installer.tar.bz2");
#die " Error: Couldn't get $repo/$tag/gt5.0.2-all-source-installer.tar.bz2" unless $rc == 200;
sys("wget -t 10 $repo/$tag/gt5.0.2-all-source-installer.tar.bz2");

# Get epydoc 3.0.1
sys("wget -t 10 http://latin.icis.pcz.pl/epydoc-3.0.1.tar.gz");


#my $rc = getstore("$repo/$tag/INDEX", "$srcdir/INDEX");
#die " Error: Couldn't get $repo/$tag/INDEX" unless $rc == 200;
sys("wget -t 10 $repo/$tag/INDEX");

open INDEX, "<INDEX" or die "Couldn't open $srcdir/INDEX to read";

while(<INDEX>) {
    my @pack = split ";;", $_;

    # Download
    if ($pack[0] eq "LF") {
	print "Getting $repo/$tag/$pack[2] to $srcdir/$pack[2]\n";
#	my $rc = getstore("$repo/$tag/$pack[2]", "$srcdir/$pack[2]");
#	die " Error: Couldn't get $repo/$tag/$pack[2]" unless $rc == 200;
	if ($pack[1] eq "BOOST") {
	    sys("wget -t 10 http://downloads.sourceforge.net/project/boost/boost/1.43.0/boost_1_43_0.tar.bz2");
	} else {
	    sys("wget -t 10 $repo/$tag/$pack[2]");
	}
    } elsif ($pack[0] eq "SVN") {
	sys("svn co $pack[2] $srcdir/SVN_$pack[1]");
    } else {
	die " Error: Unknown package type: $pack[0]\n";
    }
}

close INDEX;

# A simple wrapper for the function system() that logs the output to specific files and
# also prints the output to the nmi interface.
sub sys()
{
    my ($arg) = @_;
    print "\n% $arg\n\n";

    my $rc = open TMP, "$arg 2>&1 |";
    if (not defined $rc) {
	print STDERR " Error: Cannot execute the command $arg\n" .
	      " Error: Exiting NMI...\n";
	exit 1;
    }

    <TMP>;
    my $retval = $_;
    print $_;

    while (<TMP>) {
        my $line = $_;
        print $line;
    }
    close TMP;

    if ($?) {
	print STDERR " Error: the command `sh $arg` returned status $?\n" .
	      " Error: Exiting NMI...\n";
	exit 1;
    }
    return $retval;
}
