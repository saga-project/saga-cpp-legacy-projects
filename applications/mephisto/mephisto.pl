#!/usr/local/bin/perl -w

#use strict;
#use warnings;

use Archive::Tar;
use LWP::Simple;

$meph_version     = 0.1;
$meph_repository  = "http://macpro01.cct.lsu.edu/mephisto";
$meph_tmp_dir     = "/tmp/mephisto/";
$meph_install_dir = "/tmp/meph_inst/";

################################################################################
##
sub print_mephisto_logo
{
print "                       _     _     _        \n";
print "                      | |   (_)   | |       \n";
print "  _ __ ___   ___ _ __ | |__  _ ___| |_ ___  \n";
print " | '_ ` _ \\ / _ \\ '_ \\| '_ \\| / __| __/ _ \\ \n";
print " | | | | | |  __/ |_) | | | | \\__ \\ || (_) |\n";
print " |_| |_| |_|\\___| .__/|_| |_|_|___/\\__\\___/ \n";
print "                | |                         \n";
print "                |_| $meph_version 												\n";
}
##
################################################################################

################################################################################
##
sub redirect_console
{
	my($logfile) = @_;
	
    open(OLDOUT, ">&STDOUT"); 
    open(OLDERR, ">&STDERR");
    select(OLDOUT); $| = 1;     # make unbuffered
    select(OLDERR); $| = 1;     # make unbuffered
    open(STDOUT, ">$logfile") || die "Can't redirect stdout";
    open(STDERR, ">&STDOUT") || die "Can't dup stdout";
    select(STDERR); $| = 1;     # make unbuffered
    select(STDOUT); $| = 1;     # make unbuffered
}
##
################################################################################

################################################################################
##
sub restore_console
{
    close(STDOUT);
    close(STDERR);
    open(STDOUT, ">&OLDOUT");
    open(STDERR, ">&OLDERR");
}

################################################################################
##
sub pull_package
{
  my(@package) = @_;
  
	my $meph_rep_full = $meph_repository."/".$meph_version."/packages";
	my $package_bin_path   = "$meph_rep_full/$package[1]";
	my $package_store_path = "$meph_tmp_dir/$package[1]";

	print "\n\n Processing package $package[0]\n";
	
	#try to download the package
	print "  o Downloading to $package_store_path"; $|++;
	
	my $ret_val = getstore($package_bin_path, $package_store_path);
		die "  !! Couldn't get $package_bin_path\n" unless $ret_val == 200;
		
	print " [OK]\n";
	
	#### try to unpack the package
	##
	print "  o Extracting to ";
	my $tar = Archive::Tar->new;
	$tar->setcwd( $meph_tmp_dir );
	$tar->read($package_store_path,0);
	
	@list =  $tar->list_files;
	print "$meph_tmp_dir/$list[0]"; $|++;
	
	$tar->extract();
	print " [OK]\n";
	##
	####
	
	#### cd & configure
	##
	my $configure_string = $package[2];
	$configure_string = substr($configure_string, 1, length($configure_string)-2);
	
	my $configure_logfile = "$meph_tmp_dir/$package[0].configure.log";

	my @configure_cmd = split(' ', $configure_string);	
	push(@configure_cmd, "--prefix=$meph_install_dir");
	
	## PACKAGE SPECIFIC DEPENDENCY OPTIONS ##
	if($package[0] eq "BOOST") {
		push(@configure_cmd, "--with-python-root=$meph_install_dir");
	}

	print "  o Configuring package [$configure_string] \n    logfile: $configure_logfile"; $|++;
	chdir "$meph_tmp_dir/$list[0]";
	
	#########################################
	
	redirect_console($configure_logfile);
	my $retval = system(@configure_cmd) ; 
	restore_console();
		
	die "  [FAILED]\n" unless $retval == 0;
	print " [OK]\n";
	##
	####
	
	#### build package
	##
	my $build_string = $package[3];
	$build_string = substr($build_string, 1, length($build_string)-2);
	
	my $build_logfile = "$meph_tmp_dir/$package[0].build.log";
	print "  o Building packge [$build_string] \n    logfile: $build_logfile"; $|++;
	
	my @build_cmd = split(' ', $build_string);	
	
	redirect_console($build_logfile);
	$retval = system(@build_cmd) ; 
	restore_console();
		
	die "  [FAILED]\n" unless $retval == 0;
	print " [OK]\n";

	##
	####
	
	#### installing package
	##
	my $install_string = $package[4];
	$install_string = substr($install_string, 1, length($install_string)-2);
	
	my $install_logfile = "$meph_tmp_dir/$package[0].install.log";
	print "  o Installing packge to $meph_install_dir \n    logfile: $install_logfile"; $|++;
	
	my @install_cmd = split(' ', $install_string);	
	
	redirect_console($install_logfile);
	$retval = system(@install_cmd) ; 
	restore_console();
		
	die "  [FAILED]\n" unless $retval == 0;
	print " [OK]\n";
	##
	####
	
}
##
################################################################################

################################################################################
## "MAIN"
##
print_mephisto_logo();
$numArgs = $#ARGV + 1;

if($numArgs != 1)
{
   print "\n Usage: mephisto.pl <target dir>\n\n";
   exit();
}
else
{
	$meph_install_dir = $ARGV[0];
}

my $meph_rep_full = $meph_repository."/".$meph_version."/packages";

print "\n Source repository: $meph_rep_full\n";

# Retrieve the repository index and get the paths to
# mephisto's software source packages.
my $content = get $meph_rep_full."/INDEX";
  die "Couldn't get $meph_rep_full" unless defined $content;

my @index = split("\n", $content);
foreach my $line (@index) {
  my @packages = split(";;", $line);
  print "  o $packages[0]: $packages[1]\n" ;
}

my @index2 = split("\n", $content);
foreach my $line (@index2) {
  my @packages = split(";;", $line);
  pull_package(@packages) ;
}
#
################################################################################
