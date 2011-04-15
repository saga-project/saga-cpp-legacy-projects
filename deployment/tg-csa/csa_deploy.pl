#!/usr/bin/perl -w

BEGIN {
  use strict;
}


my $CSA_HOSTS = "./csa_hosts";
my %csa_hosts = ();
my @names     = @ARGV;


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

# if no host is given on command line, we use all hosts
if ( ! scalar ( @names ) )
{
  @names = sort keys ( %csa_hosts );
}


# for each csa host, deploy SAGA
{
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

      system ("gsissh $host 'cd $path/tg-csa/ && " .
                            "svn up           && " .
                            "env CSA_LOCATION=$path make -f make.saga.csa.mk'"); 
      print "----------------------------------------------------------\n";
    }
  }
}
