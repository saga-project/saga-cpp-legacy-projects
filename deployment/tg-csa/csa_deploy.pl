#!/usr/bin/perl -w

BEGIN {
  use strict;
}


my $CSA_HOSTS = "./csa_hosts";
my $svn       = "https://svn.cct.lsu.edu/repos/saga-projects/deployment/tg-csa";
my %csa_hosts = ();
my @names     = ();
my $do_list   = 0;
my $do_check  = 0;
my $do_deploy = 0;
my $use_all   = 0;

if ( ! scalar (@ARGV) )
{
  print <<EOT;

    $0 [-l|--list] [-c|--check] [-d|--deploy] [-a|--all|host1 host2 ...]

    -l : list available target hosts
    -c : check access mechanism for given target host(s)
    -d : deploy SAGA on given target host(s)
    -a : deploy SAGA on all known target hosts.  Use listed hosts if not
         specified

EOT
}

foreach my $arg ( @ARGV )
{
  if ( $arg =~ /^(-l|--list)$/io )
  {
    $do_list = 1;
  }
  elsif ( $arg =~ /^(-c|--check)$/io )
  {
    $do_check = 1;
  }
  elsif ( $arg =~ /^(-d|--deploy)$/io )
  {
    $do_deploy = 1;
  }
  elsif ( $arg =~ /^(-a|--all)$/io )
  {
    $use_all = 1;
  }
  elsif ( $arg =~ /^-/io )
  {
    warn "WARNING: cannot parse command line flag '$arg'\n";
  }
  else
  {
    push (@names, $arg);
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
      my $name   = $1;
      my $host   = $2;
      my $path   = $3;
      my $access = $4;

      if ( exists ( $csa_hosts{$name} ) )
      {
        warn "WARNING: duplicated csa host '$name'\n"
      }

      $csa_hosts {$1}{'host'}   = $host;
      $csa_hosts {$1}{'path'}   = $path;
      $csa_hosts {$1}{'access'} = $access;
    }
    else
    {
      warn "WARNING: Cannot parse csa host line '$tmp'\n";
    }
  }
}

if ( $use_all )
{
  @names = sort keys ( %csa_hosts );
}

if ( ! scalar (@names) )
{
  if ( $do_check || $do_deploy )
  {
    die "no host targets given\n";
  }
}


# list mode simply lists the known hosts
if ( $do_list )
{
  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "name", "host", "path";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";

  foreach my $name ( sort keys (%csa_hosts) )
  {
    if ( ! exists $csa_hosts{$name} )
    {
      warn "WARNING: Do not know how to handle host '$name'\n";
    }
    else
    {
      my $host = $csa_hosts{$name}{'host'};
      my $path = $csa_hosts{$name}{'path'};
    
      printf "| %-15s | %-40s | %-35s |\n", $name, $host, $path;
    }
  }
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  print "\n";
}


# for each csa host, check the tg-csa installation itself (also check on deploy!)
if ( $do_check || $do_deploy )
{
  # just check if we are able to deploy
  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "name", "host", "path";

  foreach my $name ( @names )
  {
    if ( ! exists $csa_hosts{$name} )
    {
      print "WARNING: Do not know how to handle host $name\n";
    }
    else
    {
      my $host   = $csa_hosts{$name}{'host'};
      my $path   = $csa_hosts{$name}{'path'};
      my $access = $csa_hosts{$name}{'access'};

      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      printf "| %-15s | %-40s | %-35s |\n", $name, $host, $path;
      print "+-----------------+------------------------------------------+-------------------------------------+\n";

      if ( 0 == system ("$access $host 'test -d $path && ".
                        " (cd $path && test -d tg-csa && (cd tg-csa && svn up) || svn co $svn)'" ) )
      {
        print "ok\n" 
      }
      else
      {
        print "error\n";
      }
    }
  }
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  print "\n";

}

if ( $do_deploy )
{
  # ! check, so do the real deployment

  print "\n";
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  printf "| %-15s | %-40s | %-35s |\n", "name", "host", "path";

  foreach my $name ( @names )
  {
    if ( ! exists $csa_hosts{$name} )
    {
      warn "WARNING: Do not know how to handle host '$name'\n";
    }
    else
    {
      my $host   = $csa_hosts{$name}{'host'};
      my $path   = $csa_hosts{$name}{'path'};
      my $access = $csa_hosts{$name}{'access'};

      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      printf "| %-15s | %-40s | %-35s |\n", $name, $host, $path;
      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      print " build trunk\n";
      system ("$access $host 'cd $path/tg-csa/                        && " .
                             "svn up                                  && " .
                             "env CSA_HOST=$name                         " .
                             "    CSA_LOCATION=$path                     " .
                             "    CSA_SAGA_VERSION=trunk                 " .
                             "    make -f make.saga.csa.mk            && " . 
                             " cp $path/README*trunk* $path/tg-csa    && " . 
                             " svn ci -m \"automated update\"          ' ");
      print "\n";
      print "+-----------------+------------------------------------------+-------------------------------------+\n";
      print " build 1.5.3\n";
      system ("$access $host 'cd $path/tg-csa/                        && " .
                             "svn up                                  && " .
                             "env CSA_HOST=$name                         " .
                             "    CSA_LOCATION=$path                     " .
                             "    CSA_SAGA_VERSION=1.5.3                 " .
                             "    make -f make.saga.csa.mk            && " . 
                             " cp $path/README*1.5.3* $path/tg-csa    && " . 
                             " svn ci -m \"automated update\"          ' ");
    }
  }
  print "+-----------------+------------------------------------------+-------------------------------------+\n";
  print "\n";
}

