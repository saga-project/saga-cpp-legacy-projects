#!/usr/local/bin/perl -w

#use strict;
#use warnings;

#use Archive::Tar;
use Getopt::Long;
use LWP::Simple;

$meph_version     = 0.2;
$meph_repository  = "http://macpro01.cct.lsu.edu/mephisto";
$meph_tmp_dir     = "/tmp/meph_tmp";
$meph_install_dir = "/tmp/meph_inst/";

##############################################################################
##
sub print_mephisto_logo {
    print "                       _     _     _        \n";
    print "                      | |   (_)   | |       \n";
    print "  _ __ ___   ___ _ __ | |__  _ ___| |_ ___  \n";
    print " | '_ ` _ \\ / _ \\ '_ \\| '_ \\| / __| __/ _ \\ \n";
    print " | | | | | |  __/ |_) | | | | \\__ \\ || (_) |\n";
    print " |_| |_| |_|\\___| .__/|_| |_|_|___/\\__\\___/ \n";
    print "                | |                         \n";
    print "                |_| Simplified SAGA Deployment\n";
}
##
##############################################################################

##############################################################################
##
sub redirect_console {
    my ($logfile) = @_;

    open( OLDOUT, ">&STDOUT" );
    open( OLDERR, ">&STDERR" );
    select(OLDOUT);
    $| = 1;    # make unbuffered
    select(OLDERR);
    $| = 1;    # make unbuffered
    open( STDOUT, ">$logfile" ) || die "Can't redirect stdout";
    open( STDERR, ">&STDOUT" )  || die "Can't dup stdout";
    select(STDERR);
    $| = 1;    # make unbuffered
    select(STDOUT);
    $| = 1;    # make unbuffered
}
##
##############################################################################

##############################################################################
##
sub restore_console {
    close(STDOUT);
    close(STDERR);
    open( STDOUT, ">&OLDOUT" );
    open( STDERR, ">&OLDERR" );
}

##############################################################################
##
sub prepare_temp {
    print "\n Setting up working directory: $meph_tmp_dir \n";
}
##
##############################################################################

sub list_packages {
	my $meph_rep_full = $meph_repository . "/repository/" . $meph_version;
	print "\n Source repository: $meph_rep_full\n\n";
	
	my $content = get $meph_rep_full. "/INDEX";
	die "Couldn't get $meph_rep_full" unless defined $content;
	
	my @index = split( "\n", $content );
	foreach my $line (@index) {
	    my @packages = split( ";;", $line );
	    print "  o $packages[0]: $packages[1]\n";
	}
	print "\n";
}

##############################################################################
##
sub pull_package {
    my (@package) = @_;

    my $meph_rep_full    = $meph_repository . "/repository/" . $meph_version ;
    my $package_bin_path = "$meph_rep_full/$package[1]";
    my $package_store_path = "$meph_tmp_dir/$package[1]";


    print "\n\n Processing package $package[0]\n";
    chdir($meph_tmp_dir);

    #try to download the package
    print "  o Downloading to $package_store_path";
    $|++;

    my $ret_val = getstore( $package_bin_path, $package_store_path );
    die "  !! Couldn't get $package_bin_path\n" unless $ret_val == 200;

    print " [OK]\n";

    #### try to unpack the package
    ##
    my $unpack_logfile = "$meph_tmp_dir/$package[0].unpack.log";
    print "  o Extracting package \n    logfile: $unpack_logfile"; $|++; 

    my @untar_cmd = ('tar', 'vxzf', $package_store_path);
    redirect_console($unpack_logfile);
    $retval = system(@untar_cmd);
    restore_console();

    die "  [FAILED]\n" unless $retval == 0;

    open(TARLOG, "$meph_tmp_dir/$package[0].unpack.log"); 
    my $package_dir_name = readline(TARLOG); # This should be the base directory
    chomp($package_dir_name);
    close(TARLOG);
    chdir "$meph_tmp_dir/$package_dir_name";

    print " [OK]\n";

    #my $tar = Archive::Tar->new;
    #$tar->setcwd($meph_tmp_dir);
    #$tar->read( $package_store_path, 0 );

    #@list = $tar->list_files;
    #print "$meph_tmp_dir/$list[0]";
    #$|++;

    #$tar->extract();
    #print " [OK]\n";
    ##
    ####

    #### cd & configure
    ##
    my $configure_string = $package[2];
    $configure_string =
      substr( $configure_string, 1, length($configure_string) - 2 );

    my $configure_logfile = "$meph_tmp_dir/$package[0].configure.log";

    my @configure_cmd = split( ' ', $configure_string );
    push( @configure_cmd, "--prefix=$meph_install_dir" );

    ## PACKAGE SPECIFIC DEPENDENCY OPTIONS ##
    #
    if ( $package[0] eq "BOOST" ) {
		# make sure that boost uses this python version
        #push( @configure_cmd, "--with-python=python" );
        push( @configure_cmd, "--with-python-root=$meph_install_dir" );
        #push( @configure_cmd, "--with-python-version=2.6" );
		
		# workaround for bug in 1.39 bootstrap
        push( @configure_cmd, "--libdir=$meph_install_dir/lib");
    }
	elsif ($package[0] eq "SAGA" ) {
		push( @configure_cmd, "--with-python=$meph_install_dir" );
		push( @configure_cmd, "--with-boost=$meph_install_dir" );
		push( @configure_cmd, "--with-postgresql=$meph_install_dir" );
	}
    #
    #########################################

    print "  o Configuring package [@configure_cmd] \n    logfile: $configure_logfile";
    $|++;

    redirect_console($configure_logfile);
    my $retval = system(@configure_cmd);
    restore_console();

    die "  [FAILED]\n" unless $retval == 0;
    print " [OK]\n";
    ##
    ####

    #### build package
    ##
    my $build_string = $package[3];
    $build_string = substr( $build_string, 1, length($build_string) - 2 );

    my $build_logfile = "$meph_tmp_dir/$package[0].build.log";
    print "  o Building package [$build_string] \n    logfile: $build_logfile";
    $|++;

    my @build_cmd = split( ' ', $build_string );

    redirect_console($build_logfile);
    $retval = system(@build_cmd);
    restore_console();

    die "  [FAILED]\n" unless $retval == 0;
    print " [OK]\n";

    ##
    ####

    #### installing package
    ##
    my $install_string = $package[4];
    $install_string = substr( $install_string, 1, length($install_string) - 2 );

    my $install_logfile = "$meph_tmp_dir/$package[0].install.log";
    print "  o Installing packge to $meph_install_dir \n    logfile: $install_logfile";
    $|++;

    my @install_cmd = split( ' ', $install_string );

    redirect_console($install_logfile);
    $retval = system(@install_cmd);
    restore_console();

    die "  [FAILED]\n" unless $retval == 0;
    print " [OK]\n";
    ##
    ####

}
##
##############################################################################

##############################################################################
##
sub write_setenv() {
	open FILE, ">", "$meph_install_dir/saga-env.sh" or  die $1;
	
	print FILE "#!/bin/bash\n";
	print FILE "\n";
	print FILE "export SAGA_LOCATION=$meph_install_dir\n"; 
	print FILE "export BOOST_LOCATION=$meph_install_dir\n\n"; 
	
	print FILE "export LD_LIBRARY_PATH=$meph_install_dir/lib:\$LD_LIBRARY_PATH\n"; 
	print FILE "export DYLD_LIBRARY_PATH=$meph_install_dir/lib:\$DYLD_LIBRARY_PATH\n\n"; 
	
	print FILE "export PYTHONPATH=$meph_install_dir/lib/python2.6/site-packages/:\$PYTHONPATH\n\n";
	
	print FILE "export PATH=$meph_install_dir/bin:\$PATH\n";
	
	close FILE;
	
	print "\n  Environment setup file written to $meph_install_dir/saga-env.sh.\n\n";
}
##
##############################################################################

##############################################################################
## 
sub print_usage () {
    print "\n Usage: mephisto.pl list <options>\n";
    print "        Lists all packages that are available in the repository.\n\n";

	print "        mephisto.pl install --target-dir=<dir> <options>\n";
    print "        Installs all or just selected packages from the repository.\n\n";

    print " Options and Arguments:\n\n";
	print "      --repository=     The repository version to use. By default,\n";
	print "                        mephisto uses the latest version.\n\n";

	print "      --target-dir=     The base directory for the installation.\n";
	print "                        All selected packages will end up in here.\n\n";

	print "      --tmp-dir=        The directory mephisto should use for downloading\n";
	print "                        and building. If omitted, it defaults to /tmp/meph_tmp.\n\n";
	
	print "      --with-packages=  Comma-separated list of optional packages to\n";
	print "                        install. By default, mephisto installs all\n";
	print "                        available packages.\n\n";
}

##############################################################################
## 
sub check_options () {

	my $help        = 0;	
	my $install_dir = 0;	
	my $tmp_dir     = 0;
	my $repository  = 0;
	my $mode        = 0;
	
	# The first option HAS to be the mode (list/install/etc)
	if( @ARGV < 1)
	{
	    print_usage();
		exit();	
	}
	
	if( $ARGV[0] eq "list")
	{
		$mode = "list";
        my $retval= GetOptions('help|?'		   => \$help,
				         	   'repository=s'  => \$repository) ; 
				         	   
        if( !($repository eq 0))
		{
		    $meph_version = $repository;
        }


		list_packages();
		exit();
	}
	elsif( $ARGV[0] eq "install")
	{
		$mode = "install";
		my $retval= GetOptions('target-dir=s'  => \$install_dir, 
							   'help|?'		   => \$help,
				         	   'tmp-dir=s'     => \$tmp_dir,
				         	   'repository=s'  => \$repository) ; 
		if(!$retval)
		{
			print_usage();
			exit();	
		}
		
		if($install_dir eq 0)
		{
			print "\n You have to set the '--target-dir' argument.\n";
			print_usage();
			exit();
		}
		else
		{
			$meph_install_dir = $install_dir;
			#if(!(-w $meph_install_dir))
			#{
            #  print "\n You don't have write permission for $install_dir.\n\n";
            #  exit();
			#}
		}
		
		if( !($tmp_dir eq 0))
		{
			$meph_tmp_dir = $tmp_dir;
		}
		
		if( !($repository eq 0))
		{
		    $meph_version = $repository;
        }
	}
	else
	{
		print_usage();
		exit();
	}
	
	if ( $help ) {
	    print_usage();
		exit()
	}
 }
##
##############################################################################

##############################################################################
## "MAIN"
##

print_mephisto_logo();
check_options();

if ( !(-d $meph_tmp_dir) ) {
	mkdir($meph_tmp_dir)
		or die "\n Couldn't create tmp directory: $meph_tmp_dir\n\n";
}

    # add uuid to the directory name to
    # avoid race conditions
    $meph_tmp_dir .= "/" . time . "/" ;
    if ( !(-d $meph_tmp_dir) ) {
	mkdir($meph_tmp_dir)
		or die "\n Couldn't create tmp directory: $meph_tmp_dir\n\n";
    }


if ( !(-d $meph_install_dir) ) {
	mkdir($meph_install_dir)
		or die "\n Couldn't create install directory: $meph_install_dir\n\n";
}

if(!(-w $meph_install_dir) ) {
  print "\n You don't have write permissions for $meph_install_dir\n\n";
  exit();
}

my $meph_rep_full = $meph_repository . "/repository/" . $meph_version;

print "\n Source repository: $meph_rep_full\n";

# Retrieve the repository index and get the paths to
# mephisto's software source packages.
my $content = get $meph_rep_full. "/INDEX";
die "Couldn't get $meph_rep_full" unless defined $content;

my @index = split( "\n", $content );
foreach my $line (@index) {
    my @packages = split( ";;", $line );
    print "  o $packages[0]: $packages[1]\n";
}

my @index2 = split( "\n", $content );
foreach my $line (@index2) {
    my @packages = split( ";;", $line );
    pull_package(@packages);
}

write_setenv();

#
##############################################################################
