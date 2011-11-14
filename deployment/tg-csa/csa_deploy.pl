#!/usr/bin/perl -w

BEGIN {
  use strict;
  use Data::Dumper;

  sub help (;$);
}


my $CSA_HOSTS = "./csa_hosts";
my $CSA_PACK  = "./csa_packages";
my $ENV       = `which env`;
my $svn       = "https://svn.cct.lsu.edu/repos/saga-projects/deployment/tg-csa";
my %csa_hosts = ();
my %csa_packs = ();
my @modules   = ();
my @hosts     = ();
my $version   = "trunk";
my $do_exe    = 0;
my $do_list   = 0;
my $do_check  = 0;
my $do_deploy = 0;
my $be_strict = 0;
my $force     = 0;
my $fake      = 0;
my $do_remove = 0;
my $svnuser   = `id -un`;
my $svnpass   = "";

chomp ($svnuser);
chomp ($ENV);

if ( ! scalar (@ARGV) )
{
  help (-1);
}

while ( my $arg = shift )
{
  if ( $arg =~ /^(-l|--list)$/io )
  {
    $do_list = 1;
  }
  elsif ( $arg =~ /^(-c|--check)$/io )
  {
    $do_check = 1;
  }
  elsif ( $arg =~ /^(-v|--version)$/io )
  {
    $version = shift || "trunk";
  }
  elsif ( $arg =~ /^(-t|--target|--targets|--targethosts)$/io )
  {
    my $tmp = shift || "all";
    @hosts = split (/,/, $tmp);
  }
  elsif ( $arg =~ /^(-m|--module|--modules)$/io )
  {
    my $tmp = shift || "all";
    @modules = split (/,/, $tmp);
  }
  elsif ( $arg =~ /^(-u|--user)$/io )
  {
    $svnuser = shift || ""; 
  }
  elsif ( $arg =~ /^(-p|--pass)$/io )
  {
    $svnpass = shift || "";
  }
  elsif ( $arg =~ /^(-n|--nothing|--noop|--no)$/io )
  {
    $fake = 1;
  }
  elsif ( $arg =~ /^(-e|--error|--exit)$/io )
  {
    $be_strict = 1;
  }
  elsif ( $arg =~ /^(-f|--force)$/io )
  {
    $force = 1;
  }
  elsif ( $arg =~ /^(-r|--remove)$/io )
  {
    $do_remove = 1;
  }
  elsif ( $arg =~ /^(-d|--deploy)$/io )
  {
    $do_deploy = 1;
  }
  elsif ( $arg =~ /^(-x|--execute)$/io )
  {
    $do_exe = 1;
  }
  elsif ( $arg =~ /^(-h|--help)$/io )
  {
    help (0);
  }
  elsif ( $arg =~ /^-/io )
  {
    warn "WARNING: cannot parse command line flag '$arg'\n";
  }
  else
  {
    help (-1);
  }
}

if ( ! scalar (@hosts) )
{
  $hosts[0] = 'all';
}

if ( ! scalar (@modules) )
{
  $modules[0] = 'all';
}


my $SVNCI = "svn --no-auth-cache";
if ( $svnuser ) { $SVNCI .= " --username '$svnuser'"; }
if ( $svnpass ) { $SVNCI .= " --password '$svnpass'"; }
$SVNCI .= " ci";

# read and parse csa packages file
{
  my $tmp = ();

  open   (TMP, "<$CSA_PACK") || die "ERROR  : cannot open '$CSA_PACK': $!\n";
  @tmp = <TMP>;
  close  (TMP);
  chomp  (@tmp);

  my $tmp_version = "";

  LINE_P:
  foreach my $tmp ( @tmp )
  {
    if ( $tmp =~ /^\s*(?:#.*)?$/io )
    {
      # skip comment lines and empty lines
    }
    elsif ( $tmp =~ /^\s*version\s*:\s*(\S+)\s*$/ )
    {
      $tmp_version = $1;
      $csa_packs {$tmp_version} = ();
      next LINE_P;
    }
    elsif ( $tmp =~ /^\s*(\S+)\s+(\S+)\s*$/io )
    {
      my $tmp_module  = $1;
      my $tmp_src     = $2;

      $csa_packs{$tmp_version}{$tmp_module} = $tmp_src;
    }
    else
    {
      warn "WARNING: Cannot parse csa package line '$tmp'\n";
    }
  }


  if ( ! exists $csa_packs{$version} )
  {
    print "Version '$version' is not defined in '$CSA_PACK'\n";
    exit -1;
  }
}

# read and parse csa host file
{
  my $tmp = ();

  open   (TMP, "<$CSA_HOSTS") || die "ERROR  : cannot open '$CSA_HOSTS': $!\n";
  @tmp = <TMP>;
  close  (TMP);
  chomp  (@tmp);

  foreach my $tmp ( @tmp )
  {
    if ( $tmp =~ /^\s*(?:#.*)?$/io )
    {
      # skip comment lines and empty lines
    }
    elsif ( $tmp =~ /^\s*(\S+)\s+(\S+)\s+(\S+)\s+((?:ssh|gsissh)\s*?.*?)\s*$/io )
    {
      my $host   = $1;
      my $fqhn   = $2;
      my $path   = $3;
      my $access = $4;

      if ( exists ( $csa_hosts{$host} ) )
      {
        warn "WARNING: duplicated csa host '$host'\n"
      }

      $csa_hosts {$1}{'fqhn'}   = $fqhn;
      $csa_hosts {$1}{'path'}   = $path;
      $csa_hosts {$1}{'access'} = $access;
    }
    else
    {
      warn "WARNING: Cannot parse csa host line '$tmp'\n";
    }
  }
}


if ( grep (/all/, @modules) )
{
  @modules = grep (!/all/, @modules);
  foreach my $module ( keys %{ $csa_packs{$version} } )
  {
    push (@modules, $module);
  }
}


if ( grep (/all/, @hosts) )
{
  @hosts = grep (/all/, @hosts);
  @hosts = sort keys ( %csa_hosts );
}


if ( ! scalar (@hosts) )
{
  if ( $do_check || $do_deploy || $do_exe || $do_remove )
  {
    die "no targets given\n";
  }
}


print <<EOT;
  ----------------------------------
  hosts    : @hosts
  modules  : @modules
  version  : $version
  ----------------------------------
EOT


# list mode simply lists the known hosts
if ( $do_list )
{
  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "host", "fqhn", "path";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";

  foreach my $host ( sort keys (%csa_hosts) )
  {
    if ( ! exists $csa_hosts{$host} )
    {
      warn "WARNING: Do not know how to handle host '$host'\n";
    }
    else
    {
      my $fqhn = $csa_hosts{$host}{'fqhn'};
      my $path = $csa_hosts{$host}{'path'};
    
      printf "| %-15s | %-40s | %-35s |\n", $host, $fqhn, $path;
    }
  }
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  print "\n";
}


# for each csa host, execute some maintainance op
if ( $do_exe )
{
  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "host", "fqhn", "path";

  foreach my $host ( @hosts )
  {
    if ( ! exists $csa_hosts{$host} )
    {
      print " WARNING: Do not know how to handle host $host\n";
    }
    else
    {
      my $fqhn   = $csa_hosts{$host}{'fqhn'};
      my $path   = $csa_hosts{$host}{'path'};
      my $access = $csa_hosts{$host}{'access'};

    # my $exe    = "rm -rf $path/csa";
    # my $exe    = "chmod -R a+rX $path/";
    # my $exe    = "rm -v $path/saga/$version/gcc-`$path/csa/cpp_version`/share/saga/saga_adaptor_ssh_job.ini";
    # my $exe    = "rm -rf " .
    #              "      $path/saga/$version/gcc-`$path/csa/cpp_version`/lib/python*/site-packages/bigjob* " .
    #              "      $path/saga/$version/gcc-`$path/csa/cpp_version`/lib/python*/site-packages/saga/bigjob*";
    # my $exe    = "rm -v $path/README.saga-$version.gcc-`$path/csa/cpp_version`.$host";
      my $exe    = "cd    $path/csa/ ; svn up";

      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      printf "| %-15s | %-40s | %-35s |\n", $host, $fqhn, $path;
      print "+-----------------+------------------------------------------+-------------------------------------+\n";

      if ( $fake )
      {
        print " $access $fqhn '$exe'\n";
      }
      else
      {
        my $cmd = "$access $fqhn '$exe'";

        if ( 0 == system ($cmd) )
        {
          print " ok\n" 
        }
        else
        {
          print " error\n";
          if ( $be_strict )
          {
            exit -1;
          }
        }
      }
    }
  }
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  print "\n";

}


if ( $do_remove )
{
  # ! check, so do the real deployment

  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "host", "fqhn", "path";

  foreach my $host ( @hosts )
  {
    if ( ! exists $csa_hosts{$host} )
    {
      warn "WARNING: Do not know how to handle host '$host'\n";
    }
    else
    {
      my $fqhn   = $csa_hosts{$host}{'fqhn'};
      my $path   = $csa_hosts{$host}{'path'};
      my $access = $csa_hosts{$host}{'access'};

      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      printf "| %-15s | %-40s | %-35s |\n", $host, $fqhn, $path;
      print "+-----------------+------------------------------------------+-------------------------------------+\n";

      print " remove installation $version on $host\n";

      my $cmd = "$access $fqhn 'rm -rf $path/saga/$version/ $path/README.saga-$version.*.$host'";

      if ( $fake )
      {
        print " $cmd\n";
      }
      else
      {
        if ( 0 == system ($cmd) )
        {
          print " ok\n";
        } 
        else
        {
          print " error\n";
          if ( $be_strict )
          {
            exit -1;
          }
        }
      }
    }
  }
}



if ( $do_deploy )
{
  # ! check, so do the real deployment

  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "host", "fqhn", "path";

  foreach my $host ( @hosts )
  {
    if ( ! exists $csa_hosts{$host} )
    {
      warn "WARNING: Do not know how to handle host '$host'\n";
    }
    else
    {
      my $fqhn   = $csa_hosts{$host}{'fqhn'};
      my $path   = $csa_hosts{$host}{'path'};
      my $access = $csa_hosts{$host}{'access'};

      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      printf "| %-15s | %-40s | %-35s |\n", $host, $fqhn, $path;
      print "+-----------------+------------------------------------------+-------------------------------------+\n";

      
      foreach my $module ( @modules )
      {
        my $src = $csa_packs{$version}{$module};

        print " build $module ($version)\n";

        my $cmd = "$access $fqhn '$ENV CSA_HOST=$host                 " .
                                 "     CSA_LOCATION=$path             " .
                                 "     CSA_SAGA_VERSION=$version      " .
                                 "     CSA_SAGA_SRC=\"$src\"          " .
                                 "     CSA_SAGA_TGT=$module-$version  " .
                                 "     CSA_FORCE=$force               " .
                                 "     make -C $path/csa/             " .
                                 "          --no-print-directory      " .
                                 "          -f make.saga.csa.mk       " .
                                 "          $module    '              " ;
        if ( $fake )
        {
          print " $cmd\n";
        }
        else
        {
          if ( 0 == system ($cmd) )
          {
            print " ok\n";
          } 
          else
          {
            print " error\n";
            if ( $be_strict )
            {
              exit -1;
            }
          }
        }

        if ( $module eq "documentation" )
        {
          my $cmd = "$access $fqhn ' cd $path/csa/                               && " .
                                   " svn add doc/README*$version*$host*          && " .
                                   " svn add mod/module*$version*$host*          && " .
                                   " $SVNCI -m \"automated update\"               ' ";
          if ( $fake )
          {
            print " $cmd\n";
          }
          else
          {
            if ( 0 == system ($cmd) )
            {
              print " ok\n";
            } 
            else
            {
              print " error\n";
              if ( $be_strict )
              {
                exit -1;
              }
            }
          }
        }
        print "\n";
      }
    }
  }
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  print "\n";
}



# for each csa host, check the csa installation itself (also check on deploy!)
if ( $do_check )
{
  # just check if we are able to deploy
  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "host", "fqhn", "path";

  foreach my $host ( @hosts )
  {
    if ( ! exists $csa_hosts{$host} )
    {
      print " WARNING: Do not know how to handle host $host\n";
    }
    else
    {
      my $fqhn   = $csa_hosts{$host}{'fqhn'};
      my $path   = $csa_hosts{$host}{'path'};
      my $access = $csa_hosts{$host}{'access'};

      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      printf "| %-15s | %-40s | %-35s |\n", $host, $fqhn, $path;
      print "+-----------------+------------------------------------------+-------------------------------------+\n";

      if ( $fake )
      {
        print " $access $fqhn 'mkdir -p $path ; cd $path && test -d csa && (cd csa && svn up) || svn co $svn'\n";
      }
      else
      {
        my $cmd = "$access $fqhn 'mkdir -p $path ; " .
                  "cd $path && test -d csa && (cd csa && svn up) || svn co $svn csa; ". 
                  "$ENV CSA_HOST=$host                 " .
                  "     CSA_LOCATION=$path             " .
                  "     CSA_SAGA_VERSION=$version      " .
                  "     CSA_SAGA_CHECK=yes             " .
                  "     make -C $path/csa/             " .
                  "          --no-print-directory      " .
                  "          -f make.saga.csa.mk       " .
                  "          all'                      " ;

        if ( 0 != system ($cmd) )
        {
          print "error running csa checks\n";
          if ( $be_strict )
          {
            exit -1;
          }
        }
      }
    }
  }
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  print "\n";
}



sub help (;$)
{
  my $ret = shift || 0;

  print <<EOT;

    $0 [-h|--help] 
       [-l|--list] 
       [-c|--check] 
       [-v|--version version=all] 
       [-n|--no]
       [-e|--exit|--error]
       [-f|--force]
       [-d|--deploy]
       [-r|--remove]
       [-x|--execute] 
       [-u|--user id] 
       [-p|--pass pw] 
       [-t|--target all,host1,host2,...] 
       [-m|--module all,saga-core,readme,...] 

    -h : this help message
    -l : list available target hosts
    -c : check csa access and tooling               (default: off)
    -v : version to deploy (see csa_packages file)  (default: trunk)
    -a : deploy SAGA on all known target hosts      (default: off)
    -u : svn user id                                (default: local user id)
    -p : svn password                               (default: "")
    -n : run 'make -n' to show what *would* be done (default: off)
    -e : exit on errors                             (default: off)
    -f : force re-deploy                            (default: off)
    -d : deploy version/modules on targets          (default: off)
    -r : remove deployment on target host           (default: off)
    -x : for maintainance, use with care!           (default: off)
    -t : target hosts to deploy on                  (default: all)
    -m : modules to deploy                          (default: all)

EOT
  exit ($ret);
}

