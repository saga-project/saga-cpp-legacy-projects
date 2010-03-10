#!/usr/bin/perl -w

BEGIN {
  use strict;
  use XML::DOM::XPath;
  use Data::Dumper;

  sub usage (;$);
}

my $in  = shift || usage ("No input given");
my $out = `basename -s .xml $in`;
chomp ($out);
$out .= ".sh";

open (OUT, ">$out") || die "Cannot open target $out: $!\n";
print OUT "#!/bin/sh -x\n\n";


my $parser = new XML::DOM::Parser;
my $doc    = $parser->parsefile ($in);

my @jobs = $doc->findnodes ('/adag/job');

foreach my $job ( @jobs )
{
  my $id   = $job->getAttribute ('id');
  my $exe  = $job->getAttribute ('name');
  my @args = $job->findnodes ('argument');
  my @uses = $job->findnodes ('uses');

  if ( 1 != scalar (@args) )
  {
    die "found job node with more than one arg element\n";
  }

  my @elems  = $args[0]->getChildNodes ();
  my $args = "";

  foreach my $elem ( @elems )
  {
    my $str = $elem->toString ();
    $str =~ s/\n//iog;
    $str =~ s/^<filename file="(.+)"\/>/$1/ig;

    if ( $str !~ /^\s*$/o )
    {
      $args .= $str . " ";
    }
  }

  $args =~ s/\s\s+/ /iog;
  $jobs{$id}{'exe' } = $exe;
  $jobs{$id}{'args'} = $args;
  
  foreach my $use ( @uses )
  {
    my $file = $use->getAttribute ('file') || "unknown";
    my $type = $use->getAttribute ('link') || "unknown";


    if ( $type eq "input" )
    {
      push (@{$jobs{$id}{'inputs'}}, $file);
    }
    elsif ( $type eq "output" )
    {
      push (@{$jobs{$id}{'outputs'}}, $file);
    }
    else
    {
      die "file mode $type unknown\n";
    }
  }

  print OUT "$exe $args\n";
}

print OUT "\n";
close (OUT);





sub usage (;$)
{
  my $msg = shift || undef;

  if ( $message )
  {
    print "\n  Error: $msg\n\n";
  }

  print <<EOT;

  Usage: $0 [dax.xml] 

  The program reads the specified workflow description in DAX
  format, and creates a shell script which, when executed, runs
  the workflow nodes in correct order.  No data movements are
  attempted by the created script, so the workflow will only be
  successfully executed if all input data are available in the
  working directory.
	   
EOT

  if ( $message )
  {
    exit -1;
  }

  exit 0;
}

