#!/usr/bin/perl -w

##############################################################################
##
##  Copyright (c) 2009-2010 Ole Weidner  (oweidner <at> cct.ct.lsu.edu)
##
##  Use, modification and distribution is subject to the Boost Software
##  License, Version 1.0. (See accompanying file LICENSE or copy at
##  http://www.boost.org/LICENSE_1_0.txt)
##
##############################################################################

##############################################################################
## CHANGELOG
##
## 30/06/2010 - Ole Weidner
##   - Updated Mephisto to make it work with the new SAGA repository
##     structure. 
##
## 24/05/2010 - Ole Weidner
##   - Migrated repository to http://static.saga.cct.lsu.edu/mephisto/
##
## 10/01/2009 - Ole Weidner
##   - Migrated repository to http://faust.cct.lsu.edu/mephisto/
##   - The latest repository version dosn't need to be set in the
##     source code anymore. It will always be in mephisto/repository/latest
##   - Mephisto will install SAGA always from the latest release tag in 
##     SVN - not from local release packages
##
##############################################################################

#use strict;
#use warnings;

use Getopt::Long;
use LWP::Simple;
use File::Basename;
use Term::ANSIColor;

##############################################################################
## Variables used for input and flags
##############################################################################

$meph_version     = "latest";
$meph_repository  = "http://static.saga.cct.lsu.edu/mephisto";
$meph_tmp_dir     = "/tmp/meph_tmp." . $<;
$meph_install_dir = "/tmp/meph_inst" . $< . "/";
$boost_check 	  = "1.40.0";
$globus           = 0;
$saga_v		  = 0;
$enab		  = 0;
$dellist 	  = 0;
$delitems         = 0;
$py_v    	  = 0;
$post_gre	  = 0;
$enab_py	  = 0;
$enab_post	  = 0;
$enab_saga 	  = 0;
$test_enab        = 0;
$adaptors	  = 0;
$dev_sagatemp     = 0;
$enab_devsaga     = 0;
##############################################################################
##
sub print_mephisto_logo {
    print color 'bold blue';
    print "                       _     _     _        \n";
    print "                      | |   (_)   | |       \n";
    print "  _ __ ___   ___ _ __ | |__  _ ___| |_ ___  \n";
    print " | '_ ` _ \\ / _ \\ '_ \\| '_ \\| / __| __/ _ \\ \n";
    print " | | | | | |  __/ |_) | | | | \\__ \\ || (_) |\n";
    print " |_| |_| |_|\\___| .__/|_| |_|_|___/\\__\\___/ \n";
    print "                | |                         \n";
    print "                |_| Simplified SAGA Deployment\n";
    print color 'reset';
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
	    print "  o $packages[1]: " . basename($packages[2]) . "\n";
	}
	print "\n";
}

##############################################################################
##
sub print_green_ok {
    print color 'bold green';
    print " [OK]";
    print color 'reset'
}

##############################################################################
##
sub print_red_failed_and_die {
    print color 'bold red';
    print " [FAILED]\n";
    print color 'reset';
    die;
}

##############################################################################
##
sub pull_package {
    my (@package) = @_;
	my @no = split(/\./,  $boost_check);	  	##boost split up into individual numbers
	my @no1 = split(/\./, $globus);  		##globus split up into individual numbers
	my @no2 = split(/\./, $py_v);			##python version number split here
	my @no3 = split(/\./, $post_gre);		##postgresql version number split here
	my @no4 = split(/\./, $saga_v);			##saga version number
	my @dev = split(/\./, $dev_sagatemp[0]);	## check to verify for development saga version number
                if (@no eq 2)
                   { 
                   	$no[2] = 0;			##boost version if provided as 1.40 or 1.44 or something similar (missing third part)
			$boost_check .= ".$no[2]"; 
		   }
    my $meph_rep_full    = $meph_repository . "/repository/" . $meph_version ;
    my $package_bin_path = "$meph_rep_full/$package[2]";
    
     my $package_store_path = "$meph_tmp_dir";
    
    if (($package[0] eq "SVN") && ($enab_saga eq 1) && ($package[1] eq "SAGA")) {     ## SAGA packages link to different choice of option 
        $package_store_path .= "/SVN_$package[1]";
  	
	if ($enab_devsaga eq 1) {
        $package[2] = "https://svn.cct.lsu.edu/repos/saga/core/tags/releases/saga-core-" . $dev[0] . "." . $dev[1] . "." . $dev[2] . "-" . $dev_sagatemp[1] . "/";
        }

	else {
	$package[2] ="https://svn.cct.lsu.edu/repos/saga/core/tags/releases/saga-core-" . $no4[0] . "." . $no4[1] . "." . $no4[2] . "/";
	}
#       print "\n\n $package_bin_path\n\n";
    }
    elsif($package[0] eq "SVN") {
       $package_store_path .= "/SVN_$package[1]";
    }

    elsif ($package[1] eq "BOOST") {
  	$package_store_path .= "/$package[1]";
	$package_bin_path = "http://sourceforge.net/projects/boost/files/boost/" . $boost_check . "/boost_" . $no[0] . "_" . $no[1] . "_" . $no[2] . ".tar.gz/download";
    }
	
    elsif (($package[1] eq "GLOBUS") && ($no1[0] eq  4) && ($no1[1] eq 2) ) {
	$package_store_path .= "/$package[1]";
        $package_bin_path ="http://www-unix.globus.org/ftppub/gt" . $no1[0] . "/" . $globus . "/installers/src/gt" . $globus . "-all-source-installer.tar.bz2";
    } 
    
    elsif ($package[1] eq "GLOBUS")  {

	$package_store_path .= "/$package[1]";
        $package_bin_path ="http://www.globus.org/ftppub/gt" . $no1[0] . "/" . $no1[0] . "." . $no1[1] . "/" . $globus . "/installers/src/gt" . $globus . "-all-source-installer.tar.bz2";
    }
    elsif (($enab_py eq 1) && ($package[1] eq "PYTHON") && ($no2[0] eq 3) && ($no2[1] eq 2)) {
	$package_store_path .= "/$package[1]";
	$package_bin_path ="http://www.python.org/ftp/python/" . $no2[0] . "." . $no2[1] . "/Python-" . $no2[0] . "." . $no2[1] . "b2.tgz";
    }
    elsif (($enab_py eq 1) && ($package[1] eq "PYTHON")) {
    	$package_store_path .= "/$package[1]";
        $package_bin_path ="http://www.python.org/ftp/python/" . $no2[0] . "." . $no2[1] . "." . $no2[2] . "/Python-" . $no2[0] . "." . $no2[1] . "." . $no2[2] . ".tgz";    
    }
    elsif (($enab_post eq 1) && ($package[1] eq "POSTGRESQL")) { 
	$package_store_path .= "/$package[1]";
	$package_bin_path ="http://wwwmaster.postgresql.org/redir/198/h/source/v" . $no3[0] . "." . $no3[1] . "." . $no3[2] . "/postgresql-" . $no3[0] . "." . $no3[1] . "." . $no3[2] . ".tar.gz";		
    }
#    elsif (($enab_saga eq 1) && ($package[1] eq "SAGA")) {
#        $package_store_path .= "/$package[1]";
#        $package[2] ="https://svn.cct.lsu.edu/repos/saga/core/tags/releases/saga-core-" . $no4[0] . "." . $no4[1] . "." . $no4[2] . "/";
#	print "\n\n $package_bin_path\n\n";       
#    }
    else {
       $package_store_path .= "/$package[2]";
    }
    
    print "\n Processing package $package[1] \n\n";
if ($package[0] eq "LF") {
    print " Package path is $package_bin_path \n\n"; ##changed from $package_bin_path
   }
else {
    print " Package path is $package[2] \n\n"; 
}
 chdir($meph_tmp_dir);

    #try to download the packages
    
    if($package[0] eq "LF") # LF: Local File
    {
      print "  o Downloading to $package_store_path";
      $|++;

      my $ret_val = getstore( $package_bin_path, $package_store_path );
      die "  !! Couldn't get $package_bin_path\n" unless $ret_val == 200;

      print_green_ok(); print "\n";
    }
    elsif($package[0] eq "SVN") # LF: Subversion
    {
      print "  o SVN checkout to $package_store_path";
      
      my $checkout_logfile = "$meph_tmp_dir/$package[1].svn_checkout.log";
      my @checkout_cmd = ('svn', 'co', $package[2] , $package_store_path);
      redirect_console($checkout_logfile);
      $retval = system(@checkout_cmd);
      restore_console();

      print_red_failed_and_die() unless $retval == 0;
      
      chdir "$package_store_path";
      print_green_ok(); print "\n";
    }
    else 
    {
      die "  !! Unknown package type: $package[0]\n"
    }
    
    
    
    #### try to unpack the package
    ##
    if($package[0] eq "LF") {
      my $unpack_logfile = "$meph_tmp_dir/$package[1].unpack.log";
      print "  o Extracting package \n    logfile: $unpack_logfile"; $|++; 
	
      if ($package_store_path eq "$meph_tmp_dir/GLOBUS"){
	
      my @untar_cmd = ('tar', 'vxjf', $package_store_path);
      redirect_console($unpack_logfile);
      $retval = system(@untar_cmd);
      restore_console();

      }
      else {
      my @untar_cmd = ('tar', 'vxzf', $package_store_path);
      redirect_console($unpack_logfile);
      $retval = system(@untar_cmd);
      restore_console();
      }
      print_red_failed_and_die() unless $retval == 0;

      	open(TARLOG, "$meph_tmp_dir/$package[1].unpack.log"); 

	 while (<TARLOG>) {
		if ($. == 2){
			$package_dir_name = $_;
		            }
	}
	my @dir = split('/',$package_dir_name);
    
#my $package_dir_name = readline(TARLOG); # This should be the base directory
#print "\n\n @dir"; 
	$package_dir_name = $dir[0] . '/';     
#   chomp($package_dir_name);
#print "$package_dir_name ";
      my $result = index($package_dir_name, "x ");
#print "$result \n";
      if($result != -1) {       
        $package_dir_name = substr $package_dir_name, $result+2;
      }
      close(TARLOG);
#     print"\n\n $package_dir_name\n\n"; 
      chdir "$meph_tmp_dir/$package_dir_name";

      print_green_ok(); print "\n";
    }

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
    my $configure_string = $package[3];
#print"\n$configure_string\n";

  $configure_string =
      substr( $configure_string, 1, length($configure_string) - 2 );
#print"\n$configure_string\n\n";
my $configure_logfile = "$meph_tmp_dir/$package[1].configure.log";

    my @configure_cmd = split( ' ', $configure_string );
    push( @configure_cmd, "--prefix=$meph_install_dir" );

    ## PACKAGE SPECIFIC DEPENDENCY OPTIONS ##
    #
    if ( $package[1] eq "BOOST" ) {
		# make sure that boost uses this python version
        #push( @configure_cmd, "--with-python=python" );
        push( @configure_cmd, "--with-python-root=$meph_install_dir" );
        #push( @configure_cmd, "--with-python-version=2.6" );
		
		# workaround for bug in 1.39 bootstrap
        push( @configure_cmd, "--libdir=$meph_install_dir/lib");
    }
	elsif ($package[1] eq "SAGA" ) {
		push( @configure_cmd, "--with-boost=$meph_install_dir" );
		push( @configure_cmd, "--with-postgresql=$meph_install_dir" );
		push( @configure_cmd, "--with-sqlite3=$meph_install_dir" );
	}
	elsif ($package[1] eq "SAGA-PYTHON" ) {
        #unshift( @configure_cmd, "SAGA_LOCATION=$meph_install_dir");
		push( @configure_cmd, "--with-python=$meph_install_dir" );
		push( @configure_cmd, "--with-boost=$meph_install_dir" );
	}

    #
    #########################################

    print "  o Configuring package [@configure_cmd] \n    logfile: $configure_logfile";
    $|++;

    redirect_console($configure_logfile);
    $ENV{SAGA_LOCATION} = $meph_install_dir;
    my $retval = system(@configure_cmd);
    restore_console();

    print_red_failed_and_die() unless $retval == 0;
    print_green_ok(); print "\n";
    ##
    ####

    #### build package
    ##
    my $build_string = $package[4];
    $build_string = substr( $build_string, 1, length($build_string) - 2 );

    my $build_logfile = "$meph_tmp_dir/$package[1].build.log";
    print "  o Building package [$build_string] \n    logfile: $build_logfile";
    $|++;

    my @build_cmd = split( ' ', $build_string );

    redirect_console($build_logfile);
    $retval = system(@build_cmd);
    restore_console();

    print_red_failed_and_die() unless $retval == 0;
    print_green_ok(); print "\n";

    ##
    ####

    #### installing     ##
    my $install_string = $package[5];
    $install_string = substr( $install_string, 1, length($install_string) - 2 );

    my $install_logfile = "$meph_tmp_dir/$package[1].install.log";
    print "  o Installing packge to $meph_install_dir \n    logfile: $install_logfile";
    $|++;

    my @install_cmd = split( ' ', $install_string );

    redirect_console($install_logfile);
    $retval = system(@install_cmd);
    restore_console();

    print_red_failed_and_die() unless $retval == 0;
    print_green_ok(); print "\n";
    ##
    ####

}
##
##############################################################################

##############################################################################
##
sub write_setenv() {
	open FILE, ">", "$meph_install_dir/share/saga/saga-env.sh" or  die $1;
	
	print FILE "#!/bin/bash\n";
	print FILE "\n";
	print FILE "export SAGA_LOCATION=$meph_install_dir\n"; 
	print FILE "export BOOST_LOCATION=$meph_install_dir\n\n"; 
	
	print FILE "export LD_LIBRARY_PATH=$meph_install_dir/lib:\$LD_LIBRARY_PATH\n"; 
	print FILE "export DYLD_LIBRARY_PATH=$meph_install_dir/lib:\$DYLD_LIBRARY_PATH\n\n"; 
	
	print FILE "export PYTHONPATH=$meph_install_dir/lib/python2.6.2/site-packages/:\$PYTHONPATH\n\n";
	
	print FILE "export PATH=$meph_install_dir/bin:\$PATH\n";
	
	close FILE;
	
	print "\n  Environment setup file written to $meph_install_dir/share/saga/saga-env.sh.\n\n";
}
##
##############################################################################

##############################################################################
## 
sub print_usage () {
    print "\n Usage: ";
    print color 'bold red';
    print "mephisto.pl list";
    print color 'reset';
    print " <options>\n";
    print "        Lists all packages that are available in the repository.\n\n";
    
    print color 'bold red';
	print "        mephisto.pl install ";
	print color 'reset';
	print "--target-dir=<dir> <options>\n";
    print "        Installs all packages with default version from the repository or\n";
    print "        selected packages with specified versions at the command prompt.\n\n";
    
    print color 'bold red';
    print "        mephisto.pl test ";
    print color 'reset';
    print "--target-dir=<dir> <options>\n";
    print "        Same as install, but additionally runs unit and adaptor tests.\n\n";

    print " Options and Arguments:\n\n";
	print "      --repository=          The repository version to use. By default,\n";
	print "                             mephisto uses the latest version.\n";
        print "                             Please use --repository=svn_trunk\n";
	print "                             for the latest saga version at the moment\n\n";
	print "      --target-dir=          The base directory for the installation.\n"; 
	print "                             All selected packages will end up in here.\n\n";

	print "      --tmp-dir=             The directory mephisto should use for downloading\n";
	print "                             and building. If omitted, it defaults to \n";
	print "                             /tmp/meph_tmp.\n\n";
	
#	print "      --with-packages=       Comma-separated list of optional packages to\n";
#	print "                             install. By default, mephisto installs all\n";
#	print "                             available packages.\n\n";
	
	print "      --with-boost-version=  Overrides the Boost version defined in the\n";
	print "                             repository. The version number should be \n";
	print "                             specified as 'x.yy.z' (must be >= 1.33.1).\n\n";
	
	print "      --with-globus-version= Overrides the Globus version defined in the\n";
	print "                             repository. The version number should be \n";
	print "                             specified as 'x.y.z' (must be >= 4.0).\n\n";   
	
	print "      --with-saga-version=   Overrides the Saga version defined in the\n";
	print "                             repository. The version number should be \n";
	print "                             specified as 'x.y.z' (must be >= 1.4).\n\n";   
	print "      --with-adaptors=  choose saga adaptors to be included.\n";
	print "                             If globus mentioned here,by default globus,\n";
	print "                             x509 and globus-adaptors will be installed\n\n"; 
	
	#print "      --exclude=                 packages to be excluded from the downloads, choose from the list with commas separated(capitals only):PYTHON, BOOST, POSTGRESQL,SQLITE,SAGA,SAGA-PYTHON,SAGA-ADAPTORS-X509,\n";
    #print "	                                SAGA-ADAPTORS-SSH\n\n";
	#print "      Recommended: complete install      \n\n"; 

}
##########################################################################################
###Test function added to perform the test after installations. Log file is created ater that 
##########################################################################################
sub test_perform()
{
    my @test_cmd = ("make check");
    my $test_dir = $meph_tmp_dir; 
    print "\n Testing SAGA installaions...";
	$test_dir .= "/SVN_SAGA";
#print "\n\n $test_dir \n\n";
    	chdir "$test_dir";
    my $test_logfile = "$test_dir/SVN_TEST.log";
   
    print "\n Logfile :  $test_logfile "; 
    redirect_console($test_logfile);
    $retval = system(@test_cmd);
    restore_console();

    print_red_failed_and_die() unless $retval == 0;
    print_green_ok(); print "\n";

}


##############################################################################
## 
sub check_options () {

	my $help        = 0;	
	my $install_dir = 0;	
	my $tmp_dir     = 0;
	my $repository  = 0;
	my $mode        = 0;
	my $v   = 0;
	my $temp2 = 0;
	my $temp = 0;		
	
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
		my $retval= GetOptions(            'target-dir=s'  		=> \$install_dir, 
							  'help|?' 		=> \$help,
				         	   'tmp-dir=s'     		=> \$tmp_dir,
				         	   'repository=s'  		=> \$repository,
						   'with-python-version=s'      => \$py_v,
						   'with-boost-version=s'  	=> \$boost_check,
						   'with-globus-version=s' 	=> \$globus,
						   'with-postgresql-version=s'  => \$post_gre,
						   'with-saga-version=s'   	=> \$saga_v,
						   'exclude=s'	   		=> \$dellist,
						   'with-adaptors=s'            => \$adaptors ) ; 
                                                                         
		@delitems = split(",",$dellist);	
		@dev_sagatemp = split("-",$saga_v);
	#	print "\n @dev_sagatemp\n";		
		if(!$retval)
		{
			print_usage();
			exit();	
		}

		if ($saga_v ne 0)
                {
                        $enab_saga= 1;
                }

		if (@dev_sagatemp eq "2")
		{
			if (($saga_v ne 0) && (($dev_sagatemp[1] eq "pre") || ($dev_sagatemp[1] eq "pre2")))
			{
				$enab_saga= 1;
				$enab_devsaga = 1;
			}
		}
		
		if ($saga_v eq "svn" || $saga_v eq "svn_trunk")
		{
		        $enab_saga= 1;
			$saga_v= "1.5.3";
		}
	
		if ($globus ne 0)
		{
			$enab= 1;
		}
		if (($adaptors ne 0) && ($adaptors eq "globus"))
		{	
			$globus = 5.0.2;	
			$enab =1;
		}
		if ($post_gre ne 0)
		{	$enab_post =1;
		}
		if ($py_v ne 0)
                {       $enab_py =1;
                }

	#	if ($saga_v eq "1.5.3")
	#	{
	#		$meph_version = "svn_trunk";
	#	}

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

	elsif( $ARGV[0] eq "test")
        {
                $mode = "test";
                my $retval= GetOptions(            'target-dir=s'               => \$install_dir,
                                                          'help|?'              => \$help,
                                                   'tmp-dir=s'                  => \$tmp_dir,
                                                   'repository=s'               => \$repository,
                                                   'with-python-version=s'      => \$py_v,
                                                   'with-boost-version=s'       => \$boost_check,
                                                   'with-globus-version=s'      => \$globus,
                                                   'with-postgresql-version=s'  => \$post_gre,
                                                   'with-saga-version=s'        => \$saga_v,
                                                   'exclude=s'                  => \$dellist,
					           'with-adaptors=s'            => \$adaptors) ;

                @delitems = split(",",$dellist);
		@dev_sagatemp = split("-",$saga_v);
		$test_enab=1;
	        if(!$retval)
                {
                        print_usage();
                        exit();
                }
		if ($saga_v ne 0)
                {
                        $enab_saga= 1;
                }

		if (@dev_sagatemp eq "2")
		{ 
			if (($saga_v ne 0) && (($dev_sagatemp[1] eq "pre") || ($dev_sagatemp[1] eq "pre2")))
                	{
                        	$enab_saga= 1;
                        	$enab_devsaga = 1;
                	}
		}

                if ($saga_v eq "svn" || $saga_v eq "svn_trunk")
                {
                        $enab_saga= 1;
                        $saga_v= "1.5.3";
                }

                if ($globus ne 0)
                {
                        $enab= 1;
                }
		if (($adaptors ne 0) && ($adaptors eq "globus"))
                {
                        $globus = 5.0.2;
                        $enab =1;
                }

                if ($post_gre ne 0)
                {       $enab_post =1;
                }
                if ($py_v ne 0)
                {       $enab_py =1;
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
		exit();
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

print "\n Source repository: $meph_rep_full\n\n";


my $content = get $meph_rep_full. "/INDEX";
die "Couldn't get $meph_rep_full" unless defined $content;

my @midpack  = ("LF","GLOBUS","$globus","'./configure'","'make prewsgram gridftp postinstall'","'make install'");
my @midpack1 = ("LF","X509","saga-adaptors-x509","'./configure'","'make'","'make install'"); 
my @midpack2 = ("LF","GLOBUS-ADAPTORS","saga-adaptors-globus","'./configure'","'make'","'make install'");

my @temp = split (/\./, $boost_check);
if (@temp eq 2) {
$boost_check .= ".0";
}

my $countp=0;
my @index = split( "\n", $content );
foreach my $line (@index) {
    my @packages = split( ";;", $line );
    if($countp eq 4 && $enab eq 1) {
    print " o $midpack[1]: http://www.globus.org/ftppub/$globus-all-source-installer.tar.bz2\n";
  #  print " o $packages[1]: $packages[2]\n";
    }
        if ($packages[1] eq "BOOST") {
   	print " o $packages[1]: http://sourceforge.net/projects/boost/files/boost/$boost_check.tar.gz/download\n";
	}
	elsif (($enab_py eq 1) && ($packages[1] eq "PYTHON")) {
	print " o $packages[1]: Python-$py_v \n";
	}
	elsif (($enab_post eq 1) && ($packages[1] eq "POSTGRESQL")) {
        print " o $packages[1]: Postgresql-$post_gre \n";
        }
	elsif (($enab_saga eq 1) && ($packages[1] eq "SAGA")) {
        print " o $packages[1]: SAGA-$saga_v \n";
        }
    	else{
    	print " o $packages[1]: $packages[2]\n";
    	}
      
	$countp = $countp + 1;
#	print"\n $countp\n"; 
  }
	if ($enab eq 1)
    {
	print " o $midpack1[1]: http://static.saga.cct.lsu.edu/software/saga-adaptors/saga-adaptors-x509-0.9.0.tgz\n";
	print " o $midpack2[1]: http://static.saga.cct.lsu.edu/software/saga-adaptors/saga-adaptors-globus-0.9.5.tgz\n";
    }

######change $count = 4 if you want  globus to be installed after sqlite and before saga
my $count = 0;
my $flag=0;
my @index2 = split( "\n", $content );
foreach my $line (@index2) {
    my @packages = split( ";;", $line );
	foreach my $iter(@delitems) 
	{
		if ($packages[1] eq $iter)
	  	   { $flag=1; 
			 }
	
	}  
	if ($flag eq 0) {	
	if ($count eq 4 && $enab eq 1) {
	pull_package(@midpack);
	}
     $count = $count + 1;
     pull_package(@packages);
	}
	else {
	if ($count eq 4 && $enab eq 1) {
        pull_package(@midpack);
	}
	$count = $count + 1;
	$flag = 0;
	}
}

######## Globus adaptors will be pulled and installed if option chosen ##########
if ($enab eq 1) {
pull_package(@midpack1);
pull_package(@midpack2);
}
#after the for loop, here packages  will store all the values as an array as was the case above
write_setenv();

####### Calling testing function after environment is written  ######  
if ($test_enab eq 1)
{ test_perform();
}

print "\n\n \t Done installations or testing (check logs for outputs and other information) :) \n\n";


##############################################################################
