#!/usr/bin/perl -w

BEGIN {
  use strict;

  sub handle_day  ($);
  sub handle_demo ($);
}

my $BASE = "http://boskop-merzky.no-ip.org/demo/";

{
  my @days = `ls -d demos-*`;

  print "Content-type: text/html\n\n";
  print <<EOT;
 <html>
  <head>
   <title> SAGA-GIN interop demos </title>
  </head>

  <body>
   <h1> SAGA-GIN interop demos </h1>
EOT

  foreach my $day ( @days )
  {
    handle_day ($day);
  }

  print <<EOT;
 </body>
EOT

}

######################################################################
#
# create a table of all demos on that day.  
# The only parameter is the directory containing the demo homes.
#
sub handle_day ($)
{
  my $day   = shift;
  my @homes = `ls $day/*`;

  foreach my $home ( @homes )
  {
    print "- <a href='$BASE/$day/$home'> $home </a> <br>\n";
  }
}


exit 0;

