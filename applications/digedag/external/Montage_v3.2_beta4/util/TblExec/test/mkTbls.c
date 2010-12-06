#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
   int cntr;

   char line[1024];

   char plate[1024];
   char ra   [1024];
   char dec  [1024];

   FILE *fin;
   FILE *fimgs;
   FILE *fjobs;

   fin = fopen("hyperAtlas.tab", "r");

   fimgs = fopen("imglist.tbl", "w+");
   fjobs = fopen("joblist.tbl", "w+");

   fprintf(fimgs,"\\fixlen = T\n");
   fprintf(fimgs, "|fname       |survey|band|crval1            |crval2|ctype1  |ctype2  |naxis1|naxis2|crpix1 |crpix2 |crota2|cdelt1      |cdelt2     |\n");

   fprintf(fjobs,"\\fixlen = T\n");
   fprintf(fjobs, "|jobid|status  |startTime               |endTime                 |return                           |\n");

   cntr = 0;

   while(1)
   {
      if(fgets(line, 1024, fin) == (char *)NULL)
	 break;

      sscanf(line, "%s %s %s", plate, ra, dec);

      fprintf(fimgs," Plate %4s J 2mass  j    %18s  %5s RA---TAN DEC--TAN   720    720   360.5   360.5    0.0  -2.777778E-4 2.777778E-4 \n",
	 plate, ra, dec);
      
      fprintf(fjobs, "  %4d                                                                                              \n", cntr);

      ++cntr;
   }

   rewind(fin);

   while(1)
   {
      if(fgets(line, 1024, fin) == (char *)NULL)
	 break;

      sscanf(line, "%s %s %s", plate, ra, dec);

      fprintf(fimgs," Plate %4s H 2mass  h    %18s  %5s RA---TAN DEC--TAN   720    720   360.5   360.5    0.0  -2.777778E-4 2.777778E-4 \n",
	 plate, ra, dec);

      fprintf(fjobs, "  %4d                                                                                              \n", cntr);

      ++cntr;
   }

   rewind(fin);

   while(1)
   {
      if(fgets(line, 1024, fin) == (char *)NULL)
	 break;

      sscanf(line, "%s %s %s", plate, ra, dec);

      fprintf(fimgs," Plate %4s K 2mass  k    %18s  %5s RA---TAN DEC--TAN   720    720   360.5   360.5    0.0  -2.777778E-4 2.777778E-4 \n",
	 plate, ra, dec);

      fprintf(fjobs, "  %4d                                                                                              \n", cntr);

      ++cntr;
   }

   rewind(fin);
   fclose(fin);
   fclose(fimgs);
   fclose(fjobs);

   exit(0);
}
