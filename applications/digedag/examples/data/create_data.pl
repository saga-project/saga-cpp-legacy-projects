#!/usr/bin/perl -w

BEGIN {
  use strict;
  use Getopt::Long;

  sub run_debug   ($);
  sub print_debug ($);
}

# autoflush
$| = 1;




###################################################################
#
# get options and print help
#
my $name   = "m101";
my $survey = "2mass";
my $band   = "j";
my $x      = 1.0;
my $y      = 1.0;
my $cdelt  = 0.000278;
my $root   = `pwd`;
my $help   = 0;
my $debug  = 0;

chomp $root;

print "\n";


my $result = GetOptions ("name=s"   => \$name,
                         "survey=s" => \$survey,
                         "band=s"   => \$band,
                         "x=s"      => \$x,
                         "y=s"      => \$y,
                         "cdelt=s"  => \$cdelt,
                         "root=s"   => \$root,
                         "help"     => \$help,  
                         "debug"    => \$debug   
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
     -debug                 prints some debug messages
     -help                  prints this message

   For more information, see man pages for mDAG and mArchiveList, 
   or the general Montage documentation.
  
EOT

  exit (! $help);
}


###################################################################
#
# main
#
my $tgt    = "$root/$name.$survey.$x.$y.$cdelt"; 
my $data   = "$root/$name.$survey.$x.$y.$cdelt/data"; 
my $list   = "$root/$name.$survey.$x.$y.$cdelt/files.tbl"; 

print_debug ("tgt dir    : '$tgt'");
print_debug ("data dir   : '$data'");
print_debug ("file list  : '$list'");

if ( ! -d $tgt  ) { run_debug ("mkdir -v $tgt "); }
if ( ! -d $data ) { run_debug ("mkdir -v $data"); }


# create dag and file list
print "creating dag ......... "; 
run_debug ("mDAG $survey $band $name $x $y $cdelt $tgt tmpurl test");
print "done\n";

print "creating file list ... "; 
run_debug ("mArchiveList $survey $band $name $x $y        $list");
print "done\n";

# grep URLs from file list, to be fetched
open (IN, "<$list") || die "Cannot open $list: $!\n";

print "download data ........ \n"; 
while ( <IN> )
{
  if ( $_ =~ /^.*\s(http\S+)\s+(\S+).*$/ ) 
  { 
    print "   $2 ... "; 
    run_debug ("wget -q '$1' -O '$data/$2'");
    print "done\n"; 
  }
}

print "download data ........ done\n"; 

print "rename hdr files ..... \n"; 
my $base = qx {grep statfile $tgt/dag.xml | grep filename | grep input | cut -f 2 -d '"'};

chomp ($base);
$base =~ s/^statfile_(.+)\.tbl$/$1/g;

print_debug ("base string: '$base'");

run_debug ("cp $tgt/region.hdr    $data/region_$base.hdr");
run_debug ("cp $tgt/cimages.tbl   $data/cimages_$base.tbl");
run_debug ("cp $tgt/pimages.tbl   $data/pimages_$base.tbl");
run_debug ("cp $tgt/statfile.tbl  $data/statfile_$base.tbl");
print "done\n";


print "\n";



###################################################################
#
# subroutines
#
sub run_debug ($)
{
  my $cmd = shift;

  if ( $debug )
  {
    print "\n - debug : running '$cmd'\n";
  }

  `$cmd`;
}

sub print_debug ($)
{
  my $msg = shift;

  if ( $debug )
  {
    print "\n - debug : $msg\n";
  }
}

