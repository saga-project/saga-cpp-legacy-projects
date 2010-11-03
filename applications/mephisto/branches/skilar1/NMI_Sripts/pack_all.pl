#! /usr/bin/env perl

use strict;
use Cwd;

my $basedir = getcwd();
my $packdir ="$basedir/packages";
exit 0;
sys('date');
chdir $packdir;
sys('pwd');
sys('ls -la');

my $packages = '';
foreach (@ARGV) {
    $packages = $packages . ' ' . $_ . '.tar.bz2';
}

sys("tar czf ../results.tar.gz $packages");
sys('date');

sub sys {
    my ($arg) = @_;
    print "\n% $arg\n\n";
    open TMP, "$arg 2>&1 |" ||
	die  "Cannot execute the command $arg\n" .
	     "Exiting...\n";
    while (<TMP>) {
	print $_;
    }
    close TMP;
    if ($?) {
	die "`sh $arg` returned status $?\n" .
	      "Exiting...\n";
    }
}

