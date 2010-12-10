#!/usr/bin/perl -w

BEGIN {
  use strict;

  use Date::Manip;
}

my $col_red    = "#FFAAAA";
my $col_green  = "#99FF99";
my $col_yellow = "#FFFF99";

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
  my @ep_logs      = `ls  $home/endpoint.*.txt 2>/dev/null`;
  my @client_logs  = `ls  $home/client.*.txt   2>/dev/null`;
  my $result_line  = `cat $home/stdout | grep -e "^result  : "`;
  my $start_line   = `cat $home/stdout | grep -e "^start   : "`;
  my $stop_line    = `cat $home/stdout | grep -e "^stop    : "`;

  $start_line =~ s/^\s*start\s*:\s*//;
  $stop_line  =~ s/^\s*stop \s*:\s*//;

  my @dates = ($start_line, $stop_line);

  print OUT <<EOT;

<html>
 <body> 
  <basefont face="mono, courier" size="4">
  <table border="0">
   <tr>
    <td> <strong> navigation </strong> </td> 
    <td> 
     <a href='../'>today</a>
     <a href='../../'>home</a>
EOT
print OUT "     <a href='../$prev/'>prev</a>\n" if $prev;
print OUT "     <a href='../$next/'>next</a>\n" if $next;
  print OUT <<EOT;
    </td> 
   </tr>
   <tr>
    <td> <strong> configuration </strong> </td> 
    <td> 
     <a href='demo.ini'>demo.ini</a>
   </tr>
   <tr>
    <td valign='top'> <strong> endpoints </strong> </td> 
    <td> 
     <table border=1
            rules=all 
            frame=box 
            cellpadding=3 
            cellspacing=0>
      <tr>
       <td bgcolor="$col_yellow"> <strong> name (ini link) </strong> </td>
       <td bgcolor="$col_yellow"> <strong> status          </strong> </td>
       <td bgcolor="$col_yellow"> <strong> jobs requested  </strong> </td>
       <td bgcolor="$col_yellow"> <strong> jobs started    </strong> </td>
       <td bgcolor="$col_yellow"> <strong> jobs registered </strong> </td>
       <td bgcolor="$col_yellow"> <strong> items requested </strong> </td>
       <td bgcolor="$col_yellow"> <strong> items completed </strong> </td>
       <td bgcolor="$col_yellow"> <strong> url             </strong> </td>
      </tr>
EOT

  my $tot_jobreq  = 0;
  my $tot_jobok   = 0;
  my $tot_jobreg  = 0;
  my $tot_itemreq = 0;
  my $tot_itemok  = 0;

  foreach my $ep ( @ep_logs )
  {
    if ( $ep =~ /(endpoint\.(.+)\.txt)$/o )
    {
      my $fname  = $1;
      my $epname = $2;

      my $url_line     = `grep -e '^  url       =' $ep`;
      my $status_line  = `grep -e '^  status    :' $ep`;
      my $jobreq_line  = `grep -e '^  #jobreq   :' $ep`;
      my $jobok_line   = `grep -e '^  #jobok    :' $ep`;
      my $jobreg_line  = `grep -e '^  #jobreg   :' $ep`;
      my $itemreq_line = `grep -e '^  #itemreq  :' $ep`;
      my $itemok_line  = `grep -e '^  #itemok   :' $ep`;

      my $url     = "-";
      my $status  = "-";
      my $jobreq  = 0;
      my $jobok   = 0;
      my $jobreg  = 0;
      my $itemreq = 0;
      my $itemok  = 0;

      if ( $url_line     =~ /^  url       =\s*(\S+)\s*$/io ) { $url     = $1; }
      if ( $status_line  =~ /^  status    :\s*(\S+)\s*$/io ) { $status  = $1; }
      if ( $jobreq_line  =~ /^  #jobreq   :\s*(\S+)\s*$/io ) { $jobreq  = $1; }
      if ( $jobok_line   =~ /^  #jobok    :\s*(\S+)\s*$/io ) { $jobok   = $1; }
      if ( $jobreg_line  =~ /^  #jobreg   :\s*(\S+)\s*$/io ) { $jobreg  = $1; }
      if ( $itemreq_line =~ /^  #itemreq  :\s*(\S+)\s*$/io ) { $itemreq = $1; }
      if ( $itemok_line  =~ /^  #itemok   :\s*(\S+)\s*$/io ) { $itemok  = $1; }

      my $col_url     = $col_yellow;
      my $col_status  = $col_yellow;
      my $col_job     = $col_yellow;
      my $col_jobreg  = $col_yellow;
      my $col_item    = $col_yellow;

      my $fail = 0;

      if ( $status  eq "ok"     ) { $col_status = $col_green; $fail = 0; }
      if ( $status  eq "failed" ) { $col_status = $col_red;   $fail = 1; }

      if ( $jobreq  == $jobok   ) { $col_job    = $col_green; } else { $col_job    = $col_red; $fail = 1; }
      if ( $jobok   == $jobreg  ) { $col_jobreg = $col_green; } else { $col_jobreg = $col_red; $fail = 1; }
      if ( $itemreq == $itemok  ) { $col_item   = $col_green; } else { $col_item   = $col_red; $fail = 1; }

      if ( ($jobreq  + $jobok ) == 0 ) { $col_job    = $col_yellow; }
      if ( ($jobreg           ) == 0 ) { $col_jobreg = $col_yellow; }
      if ( ($itemreq + $itemok) == 0 ) { $col_item   = $col_yellow; }

      if ( $jobok > $jobreg ) { $col_jobreg = $col_red; }

      if ( $itemreq == 0 ) { $fail = 1; }

      if ( $fail ) { $col_url = $col_red; } else { $col_url = $col_green; } 

      $tot_jobreq  += $jobreq ;
      $tot_jobok   += $jobok  ;
      $tot_jobreg  += $jobreg ;
      $tot_itemreq += $itemreq;
      $tot_itemok  += $itemok ;

      print OUT <<EOT;
      <tr>
       <td align="left"  bgcolor="$col_url"   > <a href='$fname'>$epname<a> </td>
       <td align="right" bgcolor="$col_status"> $status  </td>
       <td align="right" bgcolor="$col_job"   > $jobreq  </td>
       <td align="right" bgcolor="$col_job"   > $jobok   </td>
       <td align="right" bgcolor="$col_jobreg"> $jobreg  </td>
       <td align="right" bgcolor="$col_item"  > $itemreq </td>
       <td align="right" bgcolor="$col_item"  > $itemok  </td>
       <td align="left"  bgcolor="$col_url"   > $url     </td>
      </tr>
EOT
    }
  }

  print OUT <<EOT;
      <tr>
       <td align="left"  bgcolor="$col_yellow"> <strong> total        </strong> </td>
       <td align="right" bgcolor="$col_yellow"> <strong> -            </strong> </td>
       <td align="right" bgcolor="$col_yellow"> <strong> $tot_jobreq  </strong> </td>
       <td align="right" bgcolor="$col_yellow"> <strong> $tot_jobok   </strong> </td>
       <td align="right" bgcolor="$col_yellow"> <strong> $tot_jobreg  </strong> </td>
       <td align="right" bgcolor="$col_yellow"> <strong> $tot_itemreq </strong> </td>
       <td align="right" bgcolor="$col_yellow"> <strong> $tot_itemok  </strong> </td>
       <td align="left"  bgcolor="$col_yellow"> <strong> -            </strong> </td>
      </tr>
     </table>
    </td>
   </tr>
   <tr>
    <td> <strong> start: </strong> </td>
    <td> $dates[0] </td>
   </tr>
   <tr>
    <td> <strong> runtime: </strong> </td>
    <td>
EOT

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
 print OUT <<EOT;
    </td>
   </tr>
   <tr>
    <td> <strong> stdio: </strong> </td>
    <td> <a href=stdout>stdout</a> 
         <a href=stderr>stderr</a> </td>
   </tr>
   <tr>
    <td valign='top'> <strong> result: </strong> </td>
    <td> <img scale='0.5' src='./mandelbrot.png'/> </td>
   </tr>
  </table>
 </body>
</html>
EOT

}

close (OUT);


