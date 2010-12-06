#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>


void  jlSetLock(int fd, int type);


/**************************************************************/
/*                                                            */
/* This test program is meant to be used by multiple threads  */
/* and/or machines that all need to coordinate access to a    */
/* single file.  It's only purpose is to illustrate that      */
/* file locking will work for a shared file.  The nominal     */
/* use pattern is to open, e.g., terminal windows on three    */
/* machines that all see this directory, then in quick        */
/* succession run                                             */
/*                                                            */
/*      testlock 10   in the first one                        */
/*      testlock 5    in the second one                       */
/*      testlock 1    in the third one                        */
/*                                                            */
/* The second and third should pause until the first is done, */
/* and the third should continue pausing until the second     */
/* is done.                                                   */
/*                                                            */
/**************************************************************/


int main(int argc, char **argv)
{
   FILE *fp;
   int   fd;

   int   timeout;

   char  str[1024];


   if(argc < 2)
      timeout = 5;
   else
      timeout = atoi(argv[1]);

   if(timeout <= 0)
      timeout = 5;
 

   printf("Begin (timeout %2d)               %d\n", timeout, getpid());
   fflush(stdout);

   fp = fopen("testlock.dat", "w+");

   fd = fileno(fp);

   jlSetLock(fd, F_WRLCK);

   printf("File locked                      %d\n", getpid());
   fflush(stdout);
   
   printf("Writing pid to file              %d\n", getpid());
   fflush(stdout);

   sleep(timeout);

   fprintf(fp, "%d\n", getpid());
   fflush(fp);
   
   printf("Done writing; unlocking file     %d\n", getpid());
   fflush(stdout);

   jlSetLock(fd, F_UNLCK);

   printf("File unlocked; doing other stuff %d\n", getpid());
   fflush(stdout);

   sleep(timeout);

   printf("Done                             %d\n", getpid());
   fflush(stdout);
}


void jlSetLock(int fd, int type)
{
   struct flock lockinfo;

   lockinfo.l_whence = SEEK_SET;
   lockinfo.l_start  = 0;
   lockinfo.l_len    = 0;
   lockinfo.l_type   = type;

   fcntl(fd, F_SETLKW, &lockinfo);

   return;
}
