#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <joblist.h>

int main(int argc, char **argv)
{
   char job[1024];

   jlDebug(stdout);

   jlJobFile("joblist.tbl", "joblist.lock");

   jlGetJob(job);

   printf("job = [%s]\n", job);

   sleep(20);

   jlSetStatus("DONE", "Slept for 20 seconds OK");

   exit(0);
}
