#!/usr/bin/perl -w

BEGIN {
  use strict;
}


my $CSA_HOSTS = "./csa_hosts";
my %csa_hosts = ();
my @names     = ();
my $do_list   = 0;
my $do_check  = 0;
my $do_deploy = 0;
my $use_all = 0;

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
    elsif ( $tmp =~ /^\s*(\S+)\s+(\S+)\s+(\S+)\s*$/io )
    {
      my $name = $1;
      my $host = $2;
      my $path = $3;

      if ( exists ( $csa_hosts{$name} ) )
      {
        warn "WARNING: duplicated csa host '$name'\n"
      }

      $csa_hosts {$1}{'host'} = $host;
      $csa_hosts {$1}{'path'} = $path;
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
  print "----------------------------------------------------------\n";
  print "name     \t- host                       \t- path\n";
  print "----------------------------------------------------------\n";

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
    
      print "$name     \t- $host \t- $path\n";
    }
  }
  print "----------------------------------------------------------\n";
}


# for each csa host, deploy SAGA
if ( $do_check )
{
  # just check if we are able to deploy
 
  foreach my $name ( @names )
  {
    if ( ! exists $csa_hosts{$name} )
    {
      print "----------------------------------------------------------\n";
      print "check $name: Do not know how to handle entry\n";
      print "----------------------------------------------------------\n";
    }
    else
    {
      my $host = $csa_hosts{$name}{'host'};
      my $path = $csa_hosts{$name}{'path'};

      print "----------------------------------------------------------\n";
      print "check $name\t- $host \t- $path\n";

      if ( 0 == system ("gsissh $host 'cd $path/tg-csa/ && svn up'" ) )
      {
        print "ok\n" 
      }
      else
      {
        print "error\n";
      }
      print "----------------------------------------------------------\n";
    }
  }

}
elsif ( $do_deploy )
{
  # ! check, so do the real deployment

  foreach my $name ( @names )
  {
    print "----------------------------------------------------------\n";
    print "$name\n";
    print "----------------------------------------------------------\n";

    if ( ! exists $csa_hosts{$name} )
    {
      warn "WARNING: Do not know how to handle host '$name'\n";
      print "----------------------------------------------------------\n";
    }
    else
    {
      my $host = $csa_hosts{$name}{'host'};
      my $path = $csa_hosts{$name}{'path'};

      print "$name\t- $host \t- $path\n";

      print "----------------------------------------------------------\n";
      print " build trunk\n";
      print "----------------------------------------------------------\n";
      system ("gsissh $host 'cd $path/tg-csa/        && " .
                            "svn up                  && " .
                            "env CSA_HOST=$name         " .
                            "    CSA_LOCATION=$path     " .
                            "    CSA_SAGA_VERSION=trunk " .
                            "    make -f make.saga.csa.mk'"); 
      print "----------------------------------------------------------\n";
      print " build 1.5.3\n";
      print "----------------------------------------------------------\n";
      system ("gsissh $host 'cd $path/tg-csa/        && " .
                            "svn up                  && " .
                            "env CSA_HOST=$name         " .
                            "    CSA_LOCATION=$path     " .
                            "    CSA_SAGA_VERSION=1.5.3 " .
                            "    make -f make.saga.csa.mk'"); 
      print "----------------------------------------------------------\n";
    }
  }
}
