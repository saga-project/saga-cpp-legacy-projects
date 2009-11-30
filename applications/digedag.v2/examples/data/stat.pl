#!/usr/bin/perl -w

BEGIN {
  use strict;
  use IO::File;
  use Statistics::Descriptive;

  sub print_stats ($$);
}

my $file  = "./experiments";
my $name  = "";
my $stats = new Statistics::Descriptive::Full ();
my $fh    = new IO::File ("$file", "r") || die "Cannot open file: $!\n";
my $go    = 0;

# main
{
  while ( <$fh> )
  {
    my $line = $_;

    chomp ($line);

    if ( $line =~ /^\s*name:\s*(.*?)\s*$/o )
    {
      print_stats ($name, $stats) if $go;

      $name  = $1;
      $stats = new Statistics::Descriptive::Full ();
      $go    = 1;
    }
    elsif ( $line =~ /^\s*\d+:\s*/o )
    {
      my @vals = split (/\s+/, $line);

      my $time = $vals[4];

      if ( $time =~ /^(\d+):(\d+)\.(\d+)$/o )
      {
        my $secs = ($1 * 60 + $2) . ".$3";
        $stats->add_data ($secs);
      }
      else
      {
        die "Cannot parse time: $time\n";
      }
    }
  }

  print_stats ($name, $stats);
}


sub print_stats ($$)
{
  my $n = shift;
  my $s = shift;

  printf "   %-30s  \&               \&  %7.1f  \&  %7.1f  \\\\\n", 
         $n,
         $s->mean (), 
         $s->standard_deviation ();

  # print "-- $n --------------------\n";
  # print "Min     : " , $s->min                (), "\n";
  # print "Max     : " , $s->max                (), "\n";
  # print "Average : " , $s->mean               (), "\n";
  # print "StdDev  : " , $s->standard_deviation (), "\n";
  # print "Median  : " , $s->median             (), "\n";
}

