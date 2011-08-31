#!/usr/bin/perl -w

BEGIN {
  use strict;
  use Data::Dumper;

  sub help (;$);
}


my $CSA_HOSTS = "./csa_hosts";
my $CSA_PACK  = "./csa_packages";
my $svn       = "https://svn.cct.lsu.edu/repos/saga-projects/deployment/tg-csa";
my %csa_hosts = ();
my %csa_packs = ();
my @names     = ();
my $do_list   = 0;
my $do_check  = 0;
my $do_deploy = 0;
my $use_all   = 0;
my $fake      = 0;
my $svnuser   = `id -un`;
my $svnpass   = "";
my @versions  = ('all');

chomp ($svnuser);

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
  elsif ( $arg =~ /^(-v|--versions)$/io )
  {
    my $tmp = shift || "all";
    @versions = split (/,/, $tmp);
  }
  elsif ( $arg =~ /^(-d|--deploy)$/io )
  {
    $do_deploy = 1;
  }
  elsif ( $arg =~ /^(-a|--all)$/io )
  {
    $use_all = 1;
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
    push (@names, $arg);
  }
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

  my $version = "";

  LINE_P:
  foreach my $tmp ( @tmp )
  {
    if ( $tmp =~ /^\s*(?:#.*)?$/io )
    {
      # skip comment lines and empty lines
    }
    elsif ( $tmp =~ /^\s*version\s*:\s*(\S+)\s*$/ )
    {
      $version = $1;
      $csa_packs {$version} = ();
      next LINE_P;
    }
    elsif ( $tmp =~ /^\s*(\S+)\s+(\S+)\s*$/io )
    {
      my $module  = $1;
      my $src     = $2;

      my @tmp = ($module, $src);

      push (@{$csa_packs {$version}}, \@tmp);
    }
    else
    {
      warn "WARNING: Cannot parse csa package line '$tmp'\n";
    }
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

      if ( $fake )
      {
        print "$access $host 'test -d $path && ".
            " (cd $path && test -d tg-csa && (cd tg-csa && svn up) || svn co $svn)'\n";
      }
      else
      {
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

      foreach my $version ( @versions )
      {
        foreach my $modver ( @{ $csa_packs{$version} } )
        {
          my $module = $modver->[0];
          my $src    = $modver->[1];

          print " build $module ($version)\n";
          if ( $fake )
          {
            print     "$access $host 'cd $path/tg-csa/                             \n" .
                                     "svn up                                       \n" .
                                     "env CSA_HOST=$name                        \\ \n" .
                                     "    CSA_LOCATION=$path                    \\ \n" .
                                     "    CSA_SAGA_VERSION=$version             \\ \n" .
                                     "    CSA_SAGA_SRC=$src                     \\ \n" .
                                     "    make -f make.saga.csa.mk $module       ' \n" ;
          }
          else
          {
            system (  "$access $host 'cd $path/tg-csa/                            && " .
                                     "svn up                                      && " .
                                     "env CSA_HOST=$name                             " .
                                     "    CSA_LOCATION=$path                         " .
                                     "    CSA_SAGA_VERSION=$version                  " .
                                     "    make -f make.saga.csa.mk $module         ' ");
                                                                               
          }

          if ( $module eq "readme" )
          {
            if ( $fake )
            {
              print   "$access $host ' cd $path/tg-csa/                           && " .
                                     " cp -v $path/README*$version* $path/tg-csa  && " . 
                                     " svn add  README*$version*$name*            && " .
                                     " $SVNCI -m \"automated update\"              ' " ;
            }
            else
            {
              system (" $access $host ' cd $path/tg-csa/                          && " .
                                      " cp -v $path/README*$version* $path/tg-csa && " . 
                                      " svn add  README*$version*$name*           && " .
                                      " $SVNCI -m \"automated update\"             ' ");
            }
          }
          print "\n";
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

    $0 [-l|--list] [-c|--check] [-v|--version version=all] [-d|--deploy] [-u|--user id] [-p|--pass pw] [-a|--all|host1 host2 ...]

    -h : this help message
    -l : list available target hosts
    -c : check access mechanism for given target host(s)
    -v : versions to deploy (see csa_packages file)
    -d : deploy SAGA on given target host(s)
    -a : deploy SAGA on all known target hosts.
         if -a is not specified, an explicit list hostnames is expected.
    -u : svn user id (defaults to local user id)
    -p : svn password
    -n : run 'make -n' to show what *would* be done

EOT
  exit ($ret);
}

