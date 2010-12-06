#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fitsio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wcs.h>

#define MAXSTR  1024

void printFitsError(int);

int  debug;


/*****************************************************/
/*                                                   */
/*  mTruncate                                        */
/*                                                   */
/*  Determine pixel statistics for a region on the   */
/*  sky in the given image.                          */
/*                                                   */
/*****************************************************/

int main(int argc, char **argv)
{
   int    i, status, nfound, ncard, morekeys, fsize;
   int    nheader, nhdrblock, ndata, ndatablock, ntot;
   int    enddata;
   long   naxes[2];
   long   bitpix;
   char   fluxfile[MAXSTR];
   FILE  *fout;

   fitsfile *fptr;

   struct stat buf;

   debug = 0;


   /* Get the file size */

   strcpy(fluxfile, argv[1]);

   if(argc > 2)
      debug = 1;

   stat(fluxfile, &buf);

   fsize = buf.st_size;

   if(debug)
   {
      printf("\nfile size            =  %d\n\n", fsize);
      fflush(stdout);
   }


   /* Read the FITS parameters */

   status = 0;

   if(fits_open_file(&fptr, fluxfile, READONLY, &status))
   {
      printf("[struct stat=\"ERROR\", msg=\"Image file %s missing or invalid FITS\"]\n",
	 fluxfile);
      exit(1);
   }

   if(fits_read_keys_lng(fptr, "NAXIS", 1, 2, naxes, &nfound, &status))
      printFitsError(status);

   if(fits_read_key(fptr, TLONG, "BITPIX", &bitpix, (char *)NULL, &status))
      printFitsError(status);

   if(fits_get_hdrspace(fptr, &ncard, &morekeys, &status))
      printFitsError(status);

   if(debug)
   {
      printf("naxes[0]             =  %ld\n",   naxes[0]);
      printf("naxes[1]             =  %ld\n",   naxes[1]);
      printf("bitpix               =  %ld\n",   bitpix);
      printf("ncard                =  %d\n",    ncard);
      printf("\n");
      fflush(stdout);
   }


   nheader = ncard * 80;

   nhdrblock = nheader / 2880;

   if(nhdrblock * 2880 < nheader)
      ++nhdrblock;

   ndata = bitpix / 8 * naxes[0] * naxes[1];

   ndatablock = ndata / 2880;

   if(ndatablock * 2880 < ndata)
      ++ndatablock;

   ntot = nhdrblock*2880 + ndatablock*2880;

   enddata = nhdrblock*2880 + ndata;

   if(debug)
   {
      printf("nheader              = %d\n", nheader);
      printf("nhdrblock            = %d\n", nhdrblock);
      printf("nheader             -> %d\n", nhdrblock*2880);
      printf("ndata                = %d\n", ndata);
      printf("ndatablock           = %d\n", ndatablock);
      printf("ndata               -> %d\n", ndatablock*2880);
      printf("header plus data     = %d\n", enddata);
      printf("\n");
      printf("ntot                 = %d\n", ntot);
   }

   fits_close_file(fptr, &status);

   if(ntot == fsize)
   {
      printf("[struct stat=\"OK\", msg=\"File is the correct size\"]\n");
      fflush(stdout);
      exit(0);
   }
   else if(ntot > fsize)
   {
      if(nhdrblock * 2880 + ndata > fsize)
      {
	 printf("[struct stat=\"ERROR\", msg=\"File too small by %d bytes for the real data\"]\n",
	    (nhdrblock * 2880 + ndata) - fsize);
	 fflush(stdout);
	 exit(0);
      }
      else
      {
	 if(!debug)
	 {
	    truncate(fluxfile, enddata);

	    fout = fopen(fluxfile, "a");

	    for(i=enddata; i<ntot; ++i)
	       fputc(0, fout);
	    
	    fclose(fout);
	 }

	 printf("[struct stat=\"OK\", msg=\"File padded with %d bytes\"]\n",
	    ntot - fsize);
	 fflush(stdout);
	 exit(0);
      }
   }
   else
   {
      if(!debug)
	 truncate(fluxfile, ntot);

      printf("[struct stat=\"OK\", msg=\"File trunctate by %d bytes\"]\n", fsize - ntot);
      fflush(stdout);
      exit(0);
   }

   exit(0);
}



/***********************************/
/*                                 */
/*  Print out FITS library errors  */
/*                                 */
/***********************************/

void printFitsError(int status)
{
   char status_str[FLEN_STATUS];

   fits_get_errstatus(status, status_str);

   printf("[struct stat=\"ERROR\", status=%d, msg=\"%s\"]\n", status, status_str);

   exit(1);
}
