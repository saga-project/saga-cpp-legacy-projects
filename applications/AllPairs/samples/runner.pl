#!/usr/bin/perl

open FILE, '+>', '/tmp/pbstemp.pbs' or die $!;
$cmd = '/work/mmicel2/saga/saga-projects/applications/AllPairs/source/worker/main';
#$cmd = '/bin/hostname';
$nodes = '1';
foreach $argnum (0 .. $#ARGV)
{
   if ($ARGV[$argnum] eq '--instances' || $ARGV[$argnum] eq '-i')
   {
      $nodes = $ARGV[$argnum + 1];
   }
   else
   {
      if($argnum > 0 && $ARGV[$argnum-1] ne '--instances' && $ARGV[$argnum-1] ne '-i')
      {
         $cmd = $cmd . ' ' . $ARGV[$argnum];
      }
   }
}

print FILE '#!/bin/sh' . "\n";
print FILE '### Job name' . "\n";
print FILE '#PBS -N imageCompare' . "\n";
print FILE '### Queue name ' . "\n";
print FILE '#PBS -q single' . "\n";
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

for ($temp = $nodes; $temp > 0; $temp--)
{
   `qsub /tmp/pbstemp.pbs &`;
}
