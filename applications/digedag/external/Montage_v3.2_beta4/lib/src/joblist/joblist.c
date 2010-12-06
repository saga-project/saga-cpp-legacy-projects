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
#include <mtbl.h>

#include <joblist.h>


void  jlSetLock(int fd, int type);


static int  jl_haveJobFile =  0;
static int  jl_jobRec      = -1;

static char jl_jobFile [JL_MAXSTR];
static char jl_lockFile[JL_MAXSTR];
static char jl_jobID   [JL_MAXSTR];

FILE       *jl_debug = (FILE *)NULL;

char        jl_errormsg[JL_MAXSTR] = "";



/*************************************************************************/
/*                                                                       */
/*                   ********************************                    */
/*                   IRSA JOB LIST MANAGEMENT LIBRARY                    */
/*                   ********************************                    */
/*                                                                       */
/*   --------                                                            */
/*   Overview                                                            */
/*   --------                                                            */
/*                                                                       */
/*   The purpose of this library is to allow the user to run several     */
/*   copies of a program in parallel (e.g. on a cluster) drawing         */
/*   from a list of instances to be run and keeping track of which have  */
/*   been completed (and their status) and which is to be run next.      */
/*                                                                       */
/*                                                                       */
/*   -----------------                                                   */
/*   Library Functions                                                   */
/*   -----------------                                                   */
/*                                                                       */
/*                                                                       */
/*      jlDebug(FILE *debug)                                             */
/*      --------------------                                             */
/*      A stream (provided by the user) where this library               */
/*      can write its debugging output.  This is optional                */
/*      and defaults to NULL (no such output).                           */
/*                                                                       */
/*                                                                       */
/*      jlJobFile(char *listfile)                                        */
/*      -------------------------                                        */
/*      Points to a job list file.                                       */
/*                                                                       */
/*                                                                       */
/*      jlGetJob(char *jobid)                                            */
/*      --------------------                                             */
/*      Find a job that has a blank or 'PENDING' status value.           */
/*      Change the status to 'RUNNING' and set the StartTime value.      */
/*      While this transaction is underway, the file is locked so        */
/*      other instances of the program (or other programs) cannot        */
/*      change it.  These transactions are short enough that other       */
/*      programs that want to start a job or update a job's status       */
/*      can wait without slowing down the overall workflow.              */
/*                                                                       */
/*      Fills the user jobid parameter with the jobid field value        */
/*      and returns an integer OK/ERROR code.                            */
/*                                                                       */
/*                                                                       */
/*      jlSetStatus(char *status, char *retstr)                          */
/*      ---------------------------------------                          */
/*      Set the status and end message for a completed job.  In          */
/*      principle the status can also be set intermediate states         */
/*      (since these transactions are stateless).  The EndTime           */
/*      value is also set.                                               */
/*                                                                       */
/*                                                                       */
/*   -----------------                                                   */
/*   Data Organization                                                   */
/*   -----------------                                                   */
/*                                                                       */
/*   Job list files are fixed-length IPAC ASCII tables.                  */
/*                                                                       */
/*   A joblist file can be anywhere; all this library does is update     */
/*   processing state information.  Information on how the job is to be  */
/*   run can be kept in some other place (e.g. in a table file of run    */
/*   parameters) which is found by the program using custom rules but    */
/*   based on the 'jobid" (often an integer index into a table file).    */
/*                                                                       */
/*   A joblist file must have the columns jobid, status, startTime,      */
/*   endTime, and return.  The widths of these columns are not fixed,    */
/*   but if they are too small the values set by jlSetStatus() may be    */
/*   truncated.  'status' should probably be at least seven characters   */
/*   and the datetime fields at least twenty-three.                      */
/*                                                                       */
/*************************************************************************/


/***********************************************************/
/* jlDebug - Turn on debugging and specify debug file.     */
/***********************************************************/

int jlDebug(FILE *debug)
{
   jl_debug = debug;

   return(JL_OK);
}


/**********************************************************/
/*                                                        */
/* jlJobFile                                              */
/*                                                        */
/* Usage is as follows:                                   */
/*                                                        */
/* jlJobFile(char *jobfile, char *lockfile)               */
/*                                                        */
/* where jobfile is the path to the table file containing */
/* the list of jobs and their status.  File locking of    */
/* the lockfile is used to ensure that only one process   */
/* at a time can modify the job file (the lockfile at     */
/* any time contains the ID of the last process to write  */
/* to the job file).                                      */
/*                                                        */
/**********************************************************/

int jlJobFile(char *jobfile, char *lockfile)
{
   if(jl_haveJobFile == 1)
      return 1;

   strcpy(jl_jobFile,  jobfile);
   strcpy(jl_lockFile, lockfile);

   jl_haveJobFile = 1;

   if(jl_debug)
   {
      fprintf(jl_debug, "jlJobFile():   jobfile:  [%s]\n",
	 jl_jobFile);
      fprintf(jl_debug, "jlJobFile():  lockfile:  [%s]\n",
	 jl_lockFile);
      fflush(jl_debug);
   }

   return 0;
}


/**********************************************************/
/*                                                        */
/* jlGetJob                                               */
/*                                                        */
/* Usage is as follows:                                   */
/*                                                        */
/* jlGetJob()                                             */
/*                                                        */
/**********************************************************/

int jlGetJob(char *jobid)
{
   int i, ncols, stat, fdtbl, foffset, offset;
   int jobIDCol, statusCol, startTimeCol;
   int fdlock;

   char statusVal   [JL_MAXSTR];
   char line        [JL_MAXSTR];
   char datetime    [JL_MAXSTR];

   char *statusPtr;
   char *startTimePtr;

   FILE *fplock;


   /* Various time value variables */

   char       buffer[256];
   int        yr, mo, day, hr, min, sec;

   time_t     curtime;
   struct tm *loctime;

   struct TBL_INFO *tbl;


   /*********************************************************/
   /* Get the current time and convert to a datetime string */
   /*********************************************************/

   curtime = time (NULL);
   loctime = localtime (&curtime);

   strftime(buffer, 256, "%Y", loctime);
   yr = atoi(buffer);

   strftime(buffer, 256, "%m", loctime);
   mo = atoi(buffer);

   strftime(buffer, 256, "%d", loctime);
   day = atoi(buffer);

   strftime(buffer, 256, "%H", loctime);
   hr = atoi(buffer);

   strftime(buffer, 256, "%M", loctime);
   min = atoi(buffer);

   strftime(buffer, 256, "%S", loctime);
   sec = atoi(buffer);

   if(loctime->tm_isdst)
      sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d PDT", 
         yr, mo, day, hr, min, sec);
   else
      sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d PST", 
         yr, mo, day, hr, min, sec);

   if(jl_debug)
   {
      fprintf(jl_debug, "jlGetJob():    datetime: [%s]\n",
	 datetime);
      fflush(jl_debug);
   }


   /**************************************************************/
   /* Read through the table file to find the next available job */
   /**************************************************************/
 
   fplock = fopen(jl_lockFile, "w+");

   fdlock = fileno(fplock);

   jlSetLock(fdlock, F_WRLCK);
   
   tWritable(1);

   ncols = topen(jl_jobFile);

   if(ncols < 0)
   {
      strcpy(jl_errormsg, "Unable to open joblist table file.");
      return(JL_ERROR);
   }

   tbl = tsave();

   fdtbl = fileno(tbl->tfile);

   if(jl_debug)
   {
      fprintf(jl_debug, "jlGetJob():    Lock log file (read lock / blocking)\n");
      fflush(jl_debug);
   }

   statusCol = tcol("status");

   if(statusCol < 0)
   {
      strcpy(jl_errormsg, "No status column in joblist file.");
      return(JL_ERROR);
   }

   if(tbl_rec[statusCol].colwd < 7)
   {
      strcpy(jl_errormsg, "Status column must be at least seven characters wide.");
      return(JL_ERROR);
   }

   jobIDCol  = tcol("jobid");

   if(jobIDCol < 0)
   {
      strcpy(jl_errormsg, "No jobid column in joblist file.");
      return(JL_ERROR);
   }
 
   startTimeCol  = tcol("startTime");

   if(startTimeCol >= 0 && tbl_rec[startTimeCol].colwd < 23)
   {
      strcpy(jl_errormsg, "Status column must be at least twenty-three characters wide.");
      return(JL_ERROR);
   }

   jl_jobRec = 0;

   while(1)
   {
      stat = tread();
 
      if(stat < 0)
         break;

      strcpy(statusVal, tval(statusCol));


      /* If we find a job to do */

      if(strlen(statusVal) == 0 
      || strcasecmp(statusVal, "PENDING") == 0)
      {
	 /* Extract the job ID */

	 strcpy(jl_jobID, tval(jobIDCol));

	 if(jl_debug)
	 {
	    fprintf(jl_debug, "jlGetJob():    job ID: [%s]\n",
	       jl_jobID);
	    fflush(jl_debug);
	 }


	 /* Seek back to the beginning of this record */

	 if(jl_debug)
	 {
	    fprintf(jl_debug, "jlGetJob():    seek to record %d\n",
	       jl_jobRec);
	    fflush(jl_debug);
	 }

	 foffset = tseek(jl_jobRec);

	 if(jl_debug)
	 {
	    fprintf(jl_debug, "jlGetJob():    foffset = %d\n",
	       foffset);
	    fflush(jl_debug);
	 }


	 /* Reconstruct the line, with the status changed to "RUNNING" */

	 strcpy(line, tbl_rec_string);

	 if(jl_debug)
	 {
	    fprintf(jl_debug, "jlGetJob():    joblist line: [%s]\n",
	       line);
	    fflush(jl_debug);
	 }

	 offset = tbl_rec[statusCol].endcol - tbl_rec[statusCol].colwd + 1; 

	 if(jl_debug)
	 {
	    fprintf(jl_debug, "jlGetJob():    offset = %d, width = %d (status)\n",
	       offset, tbl_rec[statusCol].colwd);
	    fflush(jl_debug);
	 }

	 statusPtr = line+offset;

	 for(i=0; i<tbl_rec[statusCol].colwd; ++i)
	    statusPtr[i] = ' ';
	 
	 strcpy(statusVal, "RUNNING");

	 for(i=0; i<strlen(statusVal); ++i)
	    statusPtr[i] = statusVal[i];

	 if(jl_debug)
	 {
	    fprintf(jl_debug, "jlGetJob():    line -> [%s] (status)\n",
	       line);
	    fflush(jl_debug);
	 }


	 /* And the startTime (if it exists) */

	 if(startTimeCol >= 0)
	 {
	    offset = tbl_rec[startTimeCol].endcol - tbl_rec[startTimeCol].colwd + 1; 

	    if(jl_debug)
	    {
	       fprintf(jl_debug, "jlGetJob():    offset = %d, width = %d (startTime)\n",
		  offset, tbl_rec[startTimeCol].colwd);
	       fflush(jl_debug);
	    }

	    startTimePtr = line+offset;

	    for(i=0; i<tbl_rec[startTimeCol].colwd; ++i)
	       startTimePtr[i] = ' ';
	       
	    for(i=0; i<strlen(datetime); ++i)
	       startTimePtr[i] = datetime[i];

	    if(jl_debug)
	    {
	       fprintf(jl_debug, "jlGetJob():    line -> [%s] (startTime)\n",
		  line);
	       fflush(jl_debug);
	    }
	 }


	 /* And overwrite the record */

	 lseek(fdtbl, foffset, SEEK_SET);
	 write(fdtbl, line, strlen(line));

	 tclose();

	 if(jl_debug)
	 {
	    fprintf(jl_debug, "jlGetJob():    unlock log file\n");
	    fflush(jl_debug);
	 }

	 fprintf(fplock, "%d\n", getpid());
	 fflush(fplock);

	 jlSetLock(fdlock, F_UNLCK);

	 fclose(fplock);

	 strcpy(jobid, jl_jobID);

	 return(JL_OK);
      }

      ++jl_jobRec;
   }

   tclose();

   if(jl_debug)
   {
      fprintf(jl_debug, "jlGetJob():    unlock log file\n");
      fflush(jl_debug);
   }

   fprintf(fplock, "%d\n", getpid());
   fflush(fplock);

   jlSetLock(fdlock, F_UNLCK);

   fclose(fplock);

   return(JL_EOF);
}



/**********************************************************/
/*                                                        */
/* jlSetStatus                                            */
/*                                                        */
/* Usage is as follows:                                   */
/*                                                        */
/* jlSetStatus(char *status, char *retstr)                */
/*                                                        */
/**********************************************************/

int jlSetStatus(char *statusStr, char *returnStr)
{
   int i, len, ncols, fdtbl, foffset, offset;
   int statusCol, endTimeCol, returnCol;
   int fdlock;

   char line        [JL_MAXSTR];
   char datetime    [JL_MAXSTR];

   char *statusPtr;
   char *endTimePtr;
   char *returnPtr;

   FILE *fplock;


   /* Various time value variables */

   char       buffer[256];
   int        yr, mo, day, hr, min, sec;

   time_t     curtime;
   struct tm *loctime;

   struct TBL_INFO *tbl;


   /*********************************************************/
   /* Get the current time and convert to a datetime string */
   /*********************************************************/

   curtime = time (NULL);
   loctime = localtime (&curtime);

   strftime(buffer, 256, "%Y", loctime);
   yr = atoi(buffer);

   strftime(buffer, 256, "%m", loctime);
   mo = atoi(buffer);

   strftime(buffer, 256, "%d", loctime);
   day = atoi(buffer);

   strftime(buffer, 256, "%H", loctime);
   hr = atoi(buffer);

   strftime(buffer, 256, "%M", loctime);
   min = atoi(buffer);

   strftime(buffer, 256, "%S", loctime);
   sec = atoi(buffer);

   if(loctime->tm_isdst)
      sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d PDT", 
         yr, mo, day, hr, min, sec);
   else
      sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d PST", 
         yr, mo, day, hr, min, sec);

   if(jl_debug)
   {
      fprintf(jl_debug, "jlSetStatus():    datetime: [%s]\n",
	 datetime);
      fflush(jl_debug);
   }


   /**************************************************************/
   /* Read through the table file to find the next available job */
   /**************************************************************/
 
   fplock = fopen(jl_lockFile, "w+");

   fdlock = fileno(fplock);

   jlSetLock(fdlock, F_RDLCK);

   tWritable(1);

   ncols = topen(jl_jobFile);

   tbl = tsave();

   fdtbl = fileno(tbl->tfile);

   if(ncols < 0)
   {
      strcpy(jl_errormsg, "Unable to open joblist table file.");
      return(JL_ERROR);
   }
   
   statusCol = tcol("status");

   if(statusCol < 0)
   {
      strcpy(jl_errormsg, "No status column in joblist file.");
      return(JL_ERROR);
   }

   if(tbl_rec[statusCol].colwd < 7)
   {
      strcpy(jl_errormsg, "Status column must be at least seven characters wide.");
      return(JL_ERROR);
   }

   endTimeCol  = tcol("endTime");

   if(endTimeCol >= 0 && tbl_rec[endTimeCol].colwd < 23)
   {
      strcpy(jl_errormsg, "Status column must be at least twenty-three characters wide.");
      return(JL_ERROR);
   }

   returnCol  = tcol("return");


   /* Seek back to the beginning of this record */

   if(jl_debug)
   {
      fprintf(jl_debug, "jlGetJob():    Lock log file (read lock / blocking)\n");
      fflush(jl_debug);
   }

   if(jl_debug)
   {
      fprintf(jl_debug, "jlSetStatus():    seek to record %d\n",
	 jl_jobRec);
      fflush(jl_debug);
   }

   tseek(jl_jobRec);
   tread();

   foffset = tseek(jl_jobRec);

   if(jl_debug)
   {
      fprintf(jl_debug, "jlSetStatus():    foffset = %d\n",
	 foffset);
      fflush(jl_debug);
   }


   /* Reconstruct the line, using the user-supplied status */

   strcpy(line, tbl_rec_string);

   if(jl_debug)
   {
      fprintf(jl_debug, "jlSetStatus():    joblist line: [%s]\n",
	 line);
      fflush(jl_debug);
   }

   offset = tbl_rec[statusCol].endcol - tbl_rec[statusCol].colwd + 1; 

   if(jl_debug)
   {
      fprintf(jl_debug, "jlSetStatus():    offset = %d, width = %d (status)\n",
	 offset, tbl_rec[statusCol].colwd);
      fflush(jl_debug);
   }

   statusPtr = line+offset;

   for(i=0; i<tbl_rec[statusCol].colwd; ++i)
      statusPtr[i] = ' ';
   
   len = strlen(statusStr);

   if(tbl_rec[statusCol].colwd < len)
   len = tbl_rec[statusCol].colwd;

   for(i=0; i<len; ++i)
      statusPtr[i] = statusStr[i];

   if(jl_debug)
   {
      fprintf(jl_debug, "jlSetStatus():    line -> [%s] (status)\n",
	 line);
      fflush(jl_debug);
   }


   /* the endTime (if it exists) */

   if(endTimeCol >= 0)
   {
      offset = tbl_rec[endTimeCol].endcol - tbl_rec[endTimeCol].colwd + 1; 

      if(jl_debug)
      {
	 fprintf(jl_debug, "jlSetStatus():    offset = %d, width = %d (endTime)\n",
	    offset, tbl_rec[endTimeCol].colwd);
	 fflush(jl_debug);
      }

      endTimePtr = line+offset;

      for(i=0; i<tbl_rec[endTimeCol].colwd; ++i)
	 endTimePtr[i] = ' ';
      
      for(i=0; i<strlen(datetime); ++i)
	 endTimePtr[i] = datetime[i];

      if(jl_debug)
      {
	 fprintf(jl_debug, "jlSetStatus():    line -> [%s] (endTime)\n",
	    line);
	 fflush(jl_debug);
      }
   }


   /* and the return value  */

   if(returnCol >= 0)
   {
      offset = tbl_rec[returnCol].endcol - tbl_rec[returnCol].colwd + 1; 

      if(jl_debug)
      {
	 fprintf(jl_debug, "jlSetStatus():    offset = %d, width = %d (returnStr)\n",
	    offset, tbl_rec[returnCol].colwd);
	 fflush(jl_debug);
      }

      returnPtr = line+offset;

      for(i=0; i<tbl_rec[returnCol].colwd; ++i)
	 returnPtr[i] = ' ';
      
      len = strlen(returnStr);

      if(tbl_rec[returnCol].colwd < len)
	 len = tbl_rec[returnCol].colwd;

      for(i=0; i<len; ++i)
	 returnPtr[i] = returnStr[i];

      if(jl_debug)
      {
	 fprintf(jl_debug, "jlSetStatus():    line -> [%s] (returnStr)\n",
	    line);
	 fflush(jl_debug);
      }
   }


   /* Then overwrite the record */

   lseek(fdtbl, foffset, SEEK_SET);
   write(fdtbl, line, strlen(line));

   tclose();

   if(jl_debug)
   {
      fprintf(jl_debug, "jlGetJob():    unlock log file\n");
      fflush(jl_debug);
   }

   fprintf(fplock, "%d\n", getpid());
   fflush(fplock);

   jlSetLock(fdlock, F_UNLCK);

   fclose(fplock);

   return(JL_OK);
}



/********************************************************************/
/*                                                                  */
/* jlSetLock  Adapted from:                                         */
/*            Advanced Programming in the Unix Environment          */
/*            Stevens (pp 367-382)                                  */
/*                                                                  */
/* The fcntl() function is used here for file locking. Setting the  */
/* lock consists of filling out a struct flock (declared in         */
/* fcntl.h) that describes the type of lock needed, opening the     */
/* file with the matching mode and calling fcntl() with the proper  */
/* arguments.                                                       */
/*                                                                  */
/* Field Definitions:                                               */
/*                                                                  */
/* l_whence                                                         */
/* This field determines where the l_start field starts from (it's  */
/* like an offset for the offset). It can be either SEEK_SET (for   */
/* the beginning of the file), SEEK_CUR (for the current file       */
/* position) or SEEK_END (for the end of the file).                 */
/*                                                                  */
/* l_start                                                          */
/* This is the starting offset in bytes of the lock relative to     */
/* l_whence.                                                        */
/*                                                                  */
/* l_len                                                            */
/* This is the length of the lock region in bytes that starts from  */
/* l_start which is relative to l_whence.                           */
/*                                                                  */
/* l_type                                                           */
/* This is the lock type.  We use only two modes:                   */
/* F_RDLCK (read lock) and F_WRLCK (write lock).                    */
/*                                                                  */
/* So in our usage of Set Lock below, we start at the beginning of  */
/* the file (SEEK_SET) with an offset 0 and length 0.  This means   */
/* that lock type, F_RDLCK or F_WRLCK will be applied on the entire */
/* file.                                                            */
/*                                                                  */
/* Finally, the call to fcntl() actually sets, clears or gets the   */
/* lock. The second argument to fcntl() tells it what to do with    */
/* the data passed to it in the struct flock.  In our case, the     */
/* second argument to fcntl() is F_SETLW.  This argument tells      */
/* fcntl() to attempt to obtain the lock requested in the struct    */
/* flock structure.  If the lock cannot be obtained (since someone  */
/* else has it locked already), fcntl() will wait until the lock    */
/* has cleared, then will set itself.                               */
/********************************************************************/

void jlSetLock(int fd, int type)
{
   static struct flock lockinfo;

   lockinfo.l_whence = SEEK_SET;
   lockinfo.l_start  = 0;
   lockinfo.l_len    = 0;
   lockinfo.l_type   = type;

   fcntl(fd, F_SETLKW, &lockinfo);

   return;
}
