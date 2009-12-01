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
my $help   = 0;

chomp $root;

print "\n";


my $result = GetOptions ("name=s"   => \$name,
                         "survey=s" => \$survey,
                         "band=s"   => \$band,
                         "x=s"      => \$x,
                         "y=s"      => \$y,
                         "cdelt=s"  => \$cdelt,
                         "root=s"   => \$root,
                         "help=f"   => \$help   
                       );

if ( ! $result || $help)
{
  print <<EOT;

    Usage: $0 [options]
    
    Options (with defaults given):
     -root   `pwd`/\$name.\$survey.\$x.\$y.\$cdelt
                            target directory
     -name   m101           data objstr (?)
     -survey 2mass          data survey
     -band   j              data band
     -x      1.0            data width
     -y      1.0            data height
     -cdelt  0.000278       data resolution (?)
     -help                  prints this message

   For more information, see man pages for mDAG and mArchiveList, 
   or the general Montage documentation.
  
EOT

  exit (! $help);
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
    print "done\n"; 
  }
}

print "download data ........ done\n\n"; 

