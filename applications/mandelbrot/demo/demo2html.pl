#!/usr/bin/perl -w

BEGIN {
  use strict;

  use Date::Manip;
}

my $home = shift  || die "\n\tusage: $0 <demo_dir> <prev> <next>\n\n";
my $prev = shift  || die "\n\tusage: $0 <demo_dir> <prev> <next>\n\n";
my $next = shift  || die "\n\tusage: $0 <demo_dir> <prev> <next>\n\n";
-d "$home"        || die "invalid demo home dir $home\n";
-f "$home/stdout" || die "Cannot find demo's stdout\n";
-f "$home/stderr" || die "Cannot find demo's stderr\n";

if ( $prev eq "-" ) { $prev = ""; }
if ( $next eq "-" ) { $next = ""; }

open OUT, (">$home/index.html") || die "Cannot open output file: $!\n";

{
  my @ep_lines  = `cat $home/stdout | grep -e "created endpoint"`;
  my $done_line = `cat $home/stdout | grep -e "compute: .* out of .*done"`;
  my @dates     = `cat $home/stdout | grep -e "[0-9][0-9]:[0-9][0-9]:[0-9][0-9]"`;

  print OUT <<EOT;

<html>
 <body>
  <table border="0">
   <tr>
    <td> <strong> Navigation </strong> </td> 
    <td> 
     <a href='../'>Today</a>
     <a href='../../'>Home</a>
EOT
print OUT "     <a href='../$prev'>Prev</a>\n" if $prev;
print OUT "     <a href='../$next'>Next</a>\n" if $next;
  print OUT <<EOT;
    </td> 
   </tr>
   <tr>
    <td> <strong> Configuration </strong> </td> 
    <td> 
     <a href='demo.ini'>demo.ini</a>
   </tr>
   <tr>
    <td valign='top'> <strong> Used Endpoints </strong> </td> 
    <td> 
EOT

  foreach my $l ( @ep_lines )
  {
    if ( $l =~ /'(.+)'\s+\((.+?)\)/o )
    {
      print OUT "       $1: $2<br>\n";
    }
  }

  print OUT "    </td>\n";
  print OUT "   </tr>\n";
  print OUT "   <tr>\n";
  print OUT "    <td> <strong> Start: </strong> </td>\n";
  print OUT "    <td> $dates[0] </td>\n";
  print OUT "   </tr>\n";
  print OUT "   <tr>\n";
  print OUT "    <td> <strong> Runtime: </strong> </td>\n";
  print OUT "    <td> ";

  if ( 2 == scalar (@dates) )
  {
    my $begin = ParseDate ($dates[0]);
    my $end   = ParseDate ($dates[1]);
    my $delta = DateCalc  ($begin, $end);

    print OUT Delta_Format ($delta, "%mv:%sv min");
  }
  else
  {
    print OUT "Unknown\n";
  }
  print OUT "</td>\n";
  print OUT "   </tr>\n";
  print OUT "   <tr>\n";
  print OUT "    <td> <strong> stdio: </strong> </td>\n";
  print OUT "    <td> <a href=stdout>stdout</a> \n";
  print OUT "         <a href=stderr>stderr</a> </td>\n";
  print OUT "   </tr>\n";
  print OUT "   <tr>\n";
  print OUT "    <td valign='top'> <strong> Result: </strong> </td>\n";
  print OUT "    <td> ";

  if ( $done_line =~ /compute: (\d+) out of (\d+)\s*done/io )
  {
    my $done      = $1;
    my $scheduled = $2;

    if ( $scheduled == $done )
    {
      print OUT "all work items done: $done boxes";
    }
    else
    {
      my $missing = $scheduled - $done;
      print OUT "not all work items done: " . ($missing) . "boxes missing";
    } 

    print OUT "<br>\n";
    print OUT "         <img scale='0.5' src='./mandelbrot.png'/>";
  }
  else
  {
    print OUT "Could not successfully finish work";
  }
  print OUT "    </td>\n";
  print OUT "   </tr>\n";
  print OUT "  </table>\n";
  print OUT " </body>\n";
  print OUT "</html>\n";
}

close (OUT);


