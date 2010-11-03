#! /usr/bin/env perl

use strict;
use Cwd;

my $tag = $ENV{'_NMI_SAGA_TAG'};

my $basedir = getcwd();
my $srcdir = "$basedir/sources";
my $packdir ="$basedir/packages";

sys("pwd");
sys("ls -la");
sys("ls -la $srcdir");
chdir $srcdir;
print "\n% cd $srcdir\n\n";

# Build Globus Toolkit
sys("tar xjvf gt5.0.2-all-source-installer.tar.bz2");
chdir "gt5.0.2-all-source-installer" or die "Couldn't change directory to $srcdir/gt5.0.2-all-source-installer";
print "\n% cd $srcdir/gt5.0.2-all-source-installer\n\n";
print STDERR "\n% cd $srcdir/gt5.0.2-all-source-installer\n\n";
sys("./configure --prefix=$packdir/globus-5.0.2");
sys("make");
sys("make install");
chdir($srcdir);
print STDERR "\n% cd $srcdir\n\n";
$ENV{GLOBUS_LOCATION} = "$packdir/globus-5.0.2";
#~Build Globus Toolkit

$ENV{PATH} = "$packdir/bin:$ENV{PATH}";
$ENV{LD_LIBRARY_PATH} = "$packdir/lib:$ENV{LD_LIBRARY_PATH}";	# to point out Python shared object library
$ENV{SAGA_LOCATION} = $packdir;

open INDEX, "<$srcdir/INDEX" or die "Couldn't open $srcdir/INDEX to read";

while (<INDEX>) {
    print "\n ===> " . $_ . "\n";
    my @pack = split /;;/;

    if ($pack[2] =~ m/^boost/) {
	    $pack[2] = "boost_1_43_0.tar.bz2";
    }

    # Unpack
    my $subdir;
    if ($pack[0] eq "LF") {
	if ($pack[2] =~ m/bz2$/) {
	    $subdir = sys("tar xjvf $pack[2]");
	} else {
	    $subdir = sys("tar xzvf $pack[2]");
	}
    } elsif ($pack[0] eq "SVN") {
	$subdir = "SVN_$pack[1]";
    } else {
	die " Error: Unknown package type: $pack[0]\n";
    }
    $subdir =~ s/ (\S+)$/\1/;

    # Build
    chdir "$srcdir/$subdir" or die "Couldn't change directory to $srcdir/$subdir";
    print "\n% cd $srcdir/$subdir\n\n";
    print STDERR "\n% cd $srcdir/$subdir\n\n";
    sys("pwd");
    sys("ls -la");

    for (my $i=3; $i < @pack; ++$i) {
	my $command = $pack[$i];
	$command =~ tr/"//d;
	if ($command =~ m/configure/ || $command =~ m/bootstrap/) {
	    $command .= " --prefix=$packdir/$tag";
    	    if ($pack[1] eq "BOOST") {
		$command .= " --with-python-root=$packdir/$tag";
		$command .= " --libdir=$packdir/$tag/lib";
	    } elsif ($pack[1] eq "POSTGRESQL") {
		$command .= " --without-readline";
	    } elsif ($pack[1] eq "SAGA") {
		$command .= " --with-boost=$packdir/$tag";
		$command .= " --with-postgresql=$packdir/$tag";
		$command .= " --with-sqlite3=$packdir/$tag";
	    } elsif ($pack[1] eq "SAGA-PYTHON") {
		$command .= " --with-python=$packdir/$tag";
		$command .= " --with-boost=$packdir/$tag";
	    }
	}
	sys($command);
    }

    chdir $srcdir or die "Couldn't change directory to $srcdir";
    # Build epydoc
    if ($pack[1] =~ m/^PYTHON$/) {
	sys("tar xzvf epydoc-3.0.1.tar.gz");
	chdir "$srcdir/epydoc-3.0.1" or die "Couldn't change directory to $srcdir/epydoc-3.0.1";
	print "\n% cd $srcdir/epydoc-3.8.1\n\n";
	print STDERR "\n% cd $srcdir/epydoc-3.8.1\n\n";
	sys("make install");
        chdir $srcdir or die "Couldn't change directory to $srcdir";
	print "\n% cd $srcdir\n\n";
	print STDERR "\n% cd $srcdir\n\n";
    }
}

sub sys()
{
    my ($arg) = @_;
    print "\n% $arg\n\n";
    print STDERR "\n% $arg\n\n";

    my $rc = open TMP, "$arg 2>&1 |";
    if (not defined $rc) {
	print STDERR "\nError: Cannot execute the command $arg\n" .
	      "Error: Exiting NMI...\n";
	exit 1;
    }

    my $retval = <TMP>;
    print " $retval";
    chomp($retval);

    while (<TMP>) {
        my $line = $_;
        print " $line";
    }
    close TMP;

    if ($?) {
	print STDERR "\nError: the command `sh $arg` returned status $?\n" .
	      "Error: Exiting NMI...\n";
	exit 1;
    }
    return $retval;
}
