#!/usr/bin/perl
open FILE, '+>', '/tmp/pbstemp.pbs' or die $!;
$cmd = '/work/mmicel2/saga/saga-projects/applications/AllPairs/source/worker/compare/compare';
foreach $argnum (0 .. $#ARGV)
{
   if ($ARGV[$argnum] eq '-u')
   {
      #unique id to identify job from qstat
      $uid = $ARGV[$argnum+1];
   }
   if($ARGV[$argnum] eq '-a')
   {
      #advert output of result string
      $advert = " -a $ARGV[$argnum+1]";
   }
   if ($ARGV[$argnum] eq '-y')
   {
      #staging not required for next file on cmdline
      $dest = $ARGV[$argnum+2] . $uid;
      `globus-url-copy  $ARGV[$argnum+1] $dest`;
      $cmd = $cmd . " $dest";
   }
   elsif ($ARGV[$argnum] eq '-n')
   {
      #staging required for next file on cmdline
      $cmd = $cmd . " $ARGV[$argnum+1]";
   }
}

$cmd = $cmd . $advert;

print FILE '#!/bin/sh' . "\n";
print FILE '### Job name' . "\n";
print FILE "#PBS -N imageCompare-$uid" . "\n";
print FILE '### Output' . "\n";
print FILE "#PBS -o /tmp/imageCompare-o$uid" . "\n";
print FILE '### Error' . "\n";
print FILE "#PBS -e /tmp/imageCompare-e$uid" . "\n";
print FILE '### Queue name ' . "\n";
print FILE '#PBS -q single' . "\n";
print FILE '### Allocation ' . "\n";
print FILE '#PBS -A loni_stopgap2' . "\n";
print FILE '### Number of nodes ' . "\n";
print FILE '#PBS -l nodes=1:ppn=1,walltime=00:05:00' . "\n";
print FILE '' . "\n";
print FILE '# Print the default PBS server' . "\n";
print FILE 'echo PBS default server is $PBS_DEFAULT' . "\n";
print FILE '' . "\n";
print FILE '# Print the jobs working directory and enter it.' . "\n";
print FILE 'echo Working directory is $PBS_O_WORKDIR' . "\n";
print FILE 'cd $PBS_O_WORKDIR' . "\n";
print FILE '' . "\n";
print FILE '# Print some other environment information' . "\n";
print FILE 'echo Running on host `hostname`' . "\n";
print FILE 'echo Time is `date`' . "\n";
print FILE 'echo Directory is `pwd`' . "\n";
print FILE 'echo This jobs runs on the following processors:' . "\n";
print FILE 'NODES=`cat $PBS_NODEFILE`' . "\n";
print FILE 'echo $NODES' . "\n";
print FILE '' . "\n";
print FILE '# Compute the number of processors' . "\n";
print FILE 'NPROCS=`wc -l < $PBS_NODEFILE`' . "\n";
print FILE 'echo This job has allocated $NPROCS nodes' . "\n";
print FILE '' . "\n";
print FILE '# Run hello_world' . "\n";
print FILE 'for NODE in $NODES; do' . "\n";
print FILE   'ssh $NODE "' . $cmd . '" &' . "\n";
print FILE 'done' . "\n";
print FILE '' . "\n";
print FILE '# Wait for background jobs to complete.' . "\n";
print FILE 'wait ' . "\n";

`qsub /tmp/pbstemp.pbs`;

while (`qstat -f | grep $uid`)
{
   sleep(1);
}
