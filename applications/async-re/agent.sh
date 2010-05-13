cd /work/athota1/new_bigjob/bigjob #change to the working directory

mpirun -machinefile /work/athota1/machinefile -np 4 /work/athota1/new_bigjob/bigjob/namd2 NPT.conf
