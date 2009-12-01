#!/usr/bin/perl -w

BEGIN {
  use strict;
  use Getopt::Long;
}

# autoflush
$| = 1;

my $name   = "m101";
my $survey = "2mass";
my $band   = "j";
my $x      = 1.0;
my $y      = 1.0;
my $cdelt  = 0.000278;
my $root   = `pwd`;

chomp $root;


my $result = GetOptions ("name=s"   => \$name,
                         "survey=s" => \$survey,
                         "band=s"   => \$band,
                         "x=s"      => \$x,
                         "y=s"      => \$y,
                         "cdelt=s"  => \$cdelt,
                         "root=s"   => \$root   
                       );

if ( ! $result )
{
  die "incorrect options\n";
}

my $tgt    = "$root/$name.$survey.$x.$y.$cdelt"; 
my $data   = "$root/$name.$survey.$x.$y.$cdelt/data"; 
my $list   = "$root/$name.$survey.$x.$y.$cdelt/files.tbl"; 

if ( ! -d $tgt  ) { `mkdir -v $tgt `; }
if ( ! -d $data ) { `mkdir -v $data`; }


# create dag and file list
print "creating dag ......... "; 
`mDAG         $survey $band $name $x $y $cdelt $tgt tmpurl test`;
print "done\n";

print "creating file list ... "; 
`mArchiveList $survey $band $name $x $y        $list`;
print "done\n";

# grep URLs from file list, to be fetched
open (IN, "<$list") || die "Cannot open $list: $!\n";

print "download data ........ \n"; 
while ( <IN> )
{
  if ( $_ =~ /^.*\s(http\S+)\s+(\S+).*$/ ) 
  { 
    print "   $2 ... "; 
    `wget -q '$1' -O '$data/$2'`;
    print "done \n "; 
  }
}

print "download data ........ done \n"; 

