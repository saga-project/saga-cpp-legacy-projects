#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include <mtbl.h>
#include <svc.h>

#define  MAXLEN 1024

char *mktemp(char *template);

extern char *optarg;
extern int optind, opterr;

extern int getopt(int argc, char *const *argv, const char *options);

int debug;


/*************************************************************************/
/*                                                                       */
/*  MTBLEXEC  --  Build mosaics (using mExec) for 2MASS,                 */
/*                SDSS, or DSS for all sources in an input               */
/*                table (with ra/dec, or name columns).                  */
/*                                                                       */
/*************************************************************************/

int main(int argc, char **argv, char **envp)
{
   int    i, jid, ncol, istat, ntile, count;
   int    ch, nmosaic, showMarker;
   int    local2MASS, testOnly;
   double shrink;

   int    ihdr;

   int    inaxis1;
   int    inaxis2;

   int    icrval1;
   int    icrval2;
   int    icdelt1;
   int    icdelt2;
   int    icrpix1;
   int    icrpix2;
   int    icrota2;
   int    iequinox;

   int    ictype1;
   int    ictype2;
   int    isurvey;
   int    iband;
   int    ifname;
   int    ilabel;

   int    naxis1;
   int    naxis2;

   double crval1;
   double crval2;
   double cdelt1;
   double cdelt2;
   double crpix1;
   double crpix2;
   double crota2;
   double equinox;

   char   ctype1   [MAXLEN];
   char   ctype2   [MAXLEN];
   char   survey   [MAXLEN];
   char   band     [MAXLEN];
   char   fname    [MAXLEN];
   char   label    [MAXLEN];

   char   jobid    [MAXLEN];
   char   cwd      [MAXLEN];
   char   imglist  [MAXLEN];
   char   joblist  [MAXLEN];
   char   lockfile [MAXLEN];
   char   locstr   [MAXLEN];
   char   workdir  [MAXLEN];
   char   subdir   [MAXLEN];
   char   directory[MAXLEN];
   char   cmd      [MAXLEN];
   char   hdrFile  [MAXLEN];
   char   status   [MAXLEN];
   char   msg      [MAXLEN];
   char   hostname [MAXLEN];

   char   args     [MAXLEN];

   FILE  *fp;

   static time_t currtime, start, lasttime;


   /************************/
   /* Initialization stuff */
   /************************/

   errno = 0;

   shrink = 1.;

   time(&currtime);

   start    = currtime;
   lasttime = currtime;

   svc_sigset();

   getcwd(cwd, MAXLEN);


   /*****************************/
   /* Read the input parameters */
   /*****************************/

   strcpy(workdir,  ".");
        
   debug      = 0;
   opterr     = 0;
   showMarker = 0;
   local2MASS = 0;
   testOnly   = 0;
   ntile      = 1;

   while ((ch = getopt(argc, argv, "dxStw:s:n:")) != EOF)
   {
      switch (ch)
      {
         case 'd':
            debug = 1;
            break;
	 
         case 'x':
            showMarker = 1;
            break;
	 
         case 'S':
            local2MASS = 1;
            break;
	 
         case 't':
            testOnly = 1;
            break;
	 
	 case 'w':
	    strcpy(workdir, optarg);
	    break;
	 
	 case 's':
	    shrink = atof(optarg);

	    if(shrink <= 0.)
	       shrink = 1.;
	    break;

	 case 'n':
	    ntile = atoi(optarg);

	    if(ntile <= 1)
	       ntile = 1;
	    break;

         default:
            printf("[struct stat=\"ERROR\", msg=\"Usage: %s [-d][-o directory][-x(marker)][-t(estOnly)][-s(hrink) factor][-n tiles] imglist.tbl joblist.tbl\"]\n", argv[0]);
	    fflush(stdout);
            exit(1);
            break;
      }
   }

   if(argc - optind < 2)
   {
      printf("[struct stat=\"ERROR\", msg=\"Usage: %s [-d][-o directory][-x(marker)][-t(estOnly)][-s(hrink) factor][-n tiles] imglist.tbl joblist.tbl\"]\n", argv[0]);
      fflush(stdout);
      exit(1);
   }

   strcpy(imglist, argv[optind]);
   strcpy(joblist, argv[optind+1]);
   strcpy(lockfile, joblist);
   strcat(lockfile, ".lock");

   strcpy(args, "");

   if(local2MASS)
      strcat(args, "-S ");

   if(shrink > 1.)
   {
      sprintf(cmd, "-s %-g ", shrink);
      strcat(args, cmd);
   }

   if(showMarker)
      strcat(args, "-x ");

   if(ntile > 1)
   {
      sprintf(cmd, "-n %d -m %d", ntile, ntile);
      strcat(args, cmd);
   }

   if(debug)
   {
      printf("DEBUG> imglist   = [%s]\n",  imglist);
      printf("DEBUG> joblist   = [%s]\n",  joblist);
      printf("DEBUG> workdir   = [%s]\n",  workdir);
      printf("DEBUG> args      = [%s]\n",  args);
      printf("DEBUG> testOnly  =  %d \n",  testOnly);
      fflush(stdout);

      jlDebug(stdout);
   }

   if(mkdir(workdir, 0775) < 0)
   {
      if(errno != EEXIST)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Cannot access/create directory %d\"]\n", workdir);
	 fflush(stdout);
	 exit(1);
      }
   }

   if(workdir[strlen(workdir)-1] != '/')
   {
      strcat(workdir, "/");

      if(debug)
      {
	 printf("DEBUG> workdir   = [%s]\n", workdir);
	 fflush(stdout);
      }
   }


   /***********************************/
   /* Open and analyze the table file */
   /***********************************/

   ncol = topen(imglist);

   ihdr     = tcol("hdr");
   inaxis1  = tcol("naxis1");
   inaxis2  = tcol("naxis2");
   icrval1  = tcol("crval1");
   icrval2  = tcol("crval2");
   icdelt1  = tcol("cdelt1");
   icdelt2  = tcol("cdelt2");
   icrpix1  = tcol("crpix1");
   icrpix2  = tcol("crpix2");
   icrota2  = tcol("crota2");
   iequinox = tcol("equinox");
   ictype1  = tcol("ctype1");
   ictype2  = tcol("ctype2");
   isurvey  = tcol("survey");
   iband    = tcol("band");
   ifname   = tcol("fname");
   ilabel   = tcol("label");

   tclose();

   if(ihdr < 0)
   {
      if(inaxis1 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain naxis1 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(inaxis2 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain naxis2 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(icrval1 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain crval1 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(icrval2 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain crval2 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(icdelt1 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain cdelt1 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(icdelt2 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain cdelt2 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(icrpix1 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain crpix1 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(icrpix2 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain crpix2 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(icrota2 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain crota2 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(iequinox < 0)
	 equinox = 2000.;

      if(ictype1 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain ctype1 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }

      if(ictype2 < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Table must contain ctype2 column\"]\n");
	 fflush(stdout);
	 exit(1);
      }
   }

   if(isurvey < 0)
   {
      printf("[struct stat=\"ERROR\", msg=\"Table must contain survey column\"]\n");
      fflush(stdout);
      exit(1);
   }

   if(iband < 0)
   {
      printf("[struct stat=\"ERROR\", msg=\"Table must contain band column\"]\n");
      fflush(stdout);
      exit(1);
   }

   if(ifname < 0)
   {
      printf("[struct stat=\"ERROR\", msg=\"Table must contain fname column\"]\n");
      fflush(stdout);
      exit(1);
   }


   /********************************************************/
   /* MAIN LOOP                                            */
   /*                                                      */
   /* Use the joblist table to find images that need to be */
   /* made and pull the parameters from the imglist table. */
   /* Then use mExec to create the mosaic. Stop when the   */
   /* joblist access says there are no more to do.         */
   /********************************************************/

   nmosaic = 0;

   if(jlJobFile(joblist, lockfile))
   {
      printf("[struct stat=\"ERROR\", msg=\"Problem opening job list [%s]\"]\n", joblist);
      fflush(stdout);
      exit(1);
   }

   while(1)
   {
      if(debug)
      {
	 printf("\n\nDEBUG> ======================================================\n");
	 fflush(stdout);
      }

      if(jlGetJob(jobid))
	 break;

      ncol = topen(imglist);

      if(ncol < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Problem opening image list [%s]\"]\n", imglist);
	 fflush(stdout);
	 exit(1);
      }


      jid = atoi(jobid);

      tseek(jid);

      istat = tread();

      if(istat < 0)
      {
	 printf("[struct stat=\"ERROR\", msg=\"Problem reading record %d\"]\n", jid);
	 fflush(stdout);
	 exit(1);
      }

      ++nmosaic;

      if(ihdr >= 0)
      {
         strcpy(hdrFile, tval(ihdr));

	 if(debug)
	 {
	    printf("DEBUG> hdrFile = [%s]\n", hdrFile);
	    fflush(stdout);
	 }
      }

      else
      {
	 naxis1  = atoi(tval(inaxis1));
	 naxis2  = atoi(tval(inaxis2));

	 crval1  = atof(tval(icrval1));
	 crval2  = atof(tval(icrval2));
	 cdelt1  = atof(tval(icdelt1));
	 cdelt2  = atof(tval(icdelt2));
	 crpix1  = atof(tval(icrpix1));
	 crpix2  = atof(tval(icrpix2));
	 crota2  = atof(tval(icrota2));

	 if(iequinox >= 0)
	    equinox = atof(tval(iequinox));

	 strcpy(ctype1, tval(ictype1));
	 strcpy(ctype2, tval(ictype2));
      }

      strcpy(survey, tval(isurvey));
      strcpy(band,   tval(iband));
      strcpy(fname,  tval(ifname));

      strcpy(label, "");

      if(ilabel >= 0)
         strcpy(label, tval(ilabel));

      if(strlen(label) == 0)
	 strcpy(label, fname);

      tclose();

      strcpy(directory, workdir);

      strcpy(subdir, fname);

      strcat(directory, subdir);

      if(debug)
      {
	 printf("DEBUG> directory = [%s]\n", directory);
	 fflush(stdout);
      }
      
      if(mkdir(directory, 0775) < 0)
      {
	 if(errno != EEXIST)
	 {
	    printf("[struct stat=\"ERROR\", msg=\"Cannot access/create directory [%s]\"]\n", directory);
	    fflush(stdout);
	    exit(1);
	 }
      }


      /* Generate the header file */

      count = MAXLEN;
      gethostname(hostname, count);

      if(ihdr < 0)
      {
	 sprintf(hdrFile, "%sregion_%s_%d.hdr", workdir, hostname, getpid());
	 
	 if(debug)
	 {
	    printf("DEBUG> hdrFile = [%s]\n", hdrFile);
	    fflush(stdout);
	 }

	 fp = fopen(hdrFile, "w+");

	 if(fp == (FILE *)NULL)
	 {
	    printf("[struct stat=\"ERROR\", msg=\"Cannot open header file [%s]\"]\n", hdrFile);
	    fflush(stdout);
	    exit(1);
	 }

	 fprintf(fp, "SIMPLE  = T\n"              );
	 fprintf(fp, "BITPIX  = -64\n"            );
	 fprintf(fp, "NAXIS   = 2\n"              );
	 fprintf(fp, "NAXIS1  = %d\n",     naxis1 );
	 fprintf(fp, "NAXIS2  = %d\n",     naxis2 );
	 fprintf(fp, "CTYPE1  = '%s'\n",   ctype1 );
	 fprintf(fp, "CTYPE2  = '%s'\n",   ctype2 );
	 fprintf(fp, "CRVAL1  = %11.6f\n", crval1 );
	 fprintf(fp, "CRVAL2  = %11.6f\n", crval2 );
	 fprintf(fp, "CRPIX1  = %11.6f\n", crpix1 );
	 fprintf(fp, "CRPIX2  = %11.6f\n", crpix2 );
	 fprintf(fp, "CDELT1  = %14.9f\n", cdelt1 );
	 fprintf(fp, "CDELT2  = %14.9f\n", cdelt2 );
	 fprintf(fp, "CROTA2  = %11.6f\n", crota2 );
	 fprintf(fp, "EQUINOX = %.2f\n",   equinox);

	 if(strcasecmp(survey, "2MASS") == 0)
	 {
	    if(strcasecmp(band, "J") == 0)
	       fprintf(fp, "MAGZP   = %11.6f\n", 20.9044);

	    else if(strcasecmp(band, "H") == 0)
	       fprintf(fp, "MAGZP   = %11.6f\n", 20.4871);

	    else if(strcasecmp(band, "K") == 0)
	       fprintf(fp, "MAGZP   = %11.6f\n", 19.9757);
	 }

	 fprintf(fp, "END\n"                      );

	 fflush(fp);
	 fclose(fp);

	 if(debug)
	 {
	    printf("----------------------\n");
	    printf("SIMPLE  = T\n"              );
	    printf("BITPIX  = -64\n"            );
	    printf("NAXIS   = 2\n"              );
	    printf("NAXIS1  = %d\n",     naxis1 );
	    printf("NAXIS2  = %d\n",     naxis2 );
	    printf("CTYPE1  = '%s'\n",   ctype1 );
	    printf("CTYPE2  = '%s'\n",   ctype2 );
	    printf("CRVAL1  = %11.6f\n", crval1 );
	    printf("CRVAL2  = %11.6f\n", crval2 );
	    printf("CRPIX1  = %11.6f\n", crpix1 );
	    printf("CRPIX2  = %11.6f\n", crpix2 );
	    printf("CDELT1  = %14.9f\n", cdelt1 );
	    printf("CDELT2  = %14.9f\n", cdelt2 );
	    printf("CROTA2  = %11.6f\n", crota2 );
	    printf("EQUINOX = %.2f\n",   equinox);

	    if(strcasecmp(survey, "2MASS") == 0)
	    {
	       if(strcasecmp(band, "J") == 0)
		  printf("MAGZP   = %11.6f\n", 20.9044);

	       else if(strcasecmp(band, "H") == 0)
		  printf("MAGZP   = %11.6f\n", 20.4871);

	       else if(strcasecmp(band, "K") == 0)
		  printf("MAGZP   = %11.6f\n", 19.9757);
	    }

	    printf("END\n"                      );
	    printf("----------------------\n");
	 }
      }


      /* Update the status so we know where the job is running */
      strcpy(status, "     ");
      strcat(status, " : ");
      strcat(status, hostname);
      strcat(status, " : ");
      strcat(status, directory);

      jlSetStatus("RUNNING", status);


      /* Run mExec */

      sprintf(cmd, "mExec %s -L \"%s\" -l -f %s %s %s %s",
	 args, label, hdrFile, survey, band, directory);
      
      if(debug)
      {
	 printf("DEBUG> cmd = [%s]\n", cmd);
	 fflush(stdout);
      }

      if(!testOnly)
      {
	 svc_run(cmd);

	 strcpy(status, svc_value( "stat" ));

	 if(debug)
	 {
	    printf("DEBUG> status = [%s]\n", status);
	    fflush(stdout);
	 }
      }
      else
      {
         if(debug)
	 {
	    printf("DEBUG> Test only:  sleeping 5 seconds\n");
	    fflush(stdout);
	 }

         sleep(5);
	 strcpy(status, "OK");
      }

      if (strcmp(status, "OK") == 0)
         strcat(status, "   ");

      strcat(status, " : ");
      strcat(status, hostname);
      strcat(status, " : ");
      strcat(status, directory);

      if (strncmp(status, "ERROR", 5) == 0
      ||  strncmp(status, "ABORT", 5) == 0)
      {
	 jlSetStatus("DONE", status);

	 unlink(hdrFile);
	 printf("[struct stat=\"ERROR\", nsucceed=%d]\n", nmosaic);
	 fflush(stdout);
	 exit(0);
      }

      time(&currtime);

      sprintf(msg, "%d seconds", (int)(currtime-lasttime));

      jlSetStatus("DONE", status);

      if(debug)
      {
	 printf("DEBUG> Runtime = %d sec\n", (int)(currtime-lasttime));
	 fflush(stdout);
      }

      lasttime = currtime;
   }


   unlink(hdrFile);
   printf("[struct stat=\"OK\", nsucceed=%d]\n", nmosaic);
   fflush(stdout);
   exit(0);
}
