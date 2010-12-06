/* Module: mArchiveList.c


Version  Developer        Date     Change
-------  ---------------  -------  -----------------------
1.0      John Good        14Dec04  Baseline code

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define URL  0
#define GPFS 1
#define GFTP 2
#define URI  3

#define MAXLEN 20000

char *url_encode (char *s);
int   tcp_connect(char *hostname, int port);
int   readline   (int fd, char *line) ;

int debug = 0;


/********************************************/
/*                                          */
/* mArchiveList -- Given a location on the  */
/* sky, archive name, and size in degrees   */
/* contact the IRSA server to retreive      */
/* a list of archive images.  The list      */
/* contains enough information to support   */
/* mArchiveGet downloads.                   */
/*                                          */
/********************************************/

int main(int argc, char **argv)
{
   int    socket, port, count;
  
   char   line      [MAXLEN];
   char   request   [MAXLEN];
   char   base      [MAXLEN];
   char   constraint[MAXLEN];
   char   server    [MAXLEN];
   char   source    [MAXLEN];
   char   type      [MAXLEN];

   char  *survey;
   char  *band;
   char  *locstr;
   char  *widthstr;
   char  *heightstr;

   FILE  *fout;


   /* Construct service request using location/size */

   strcpy(type, "url");

   if(argc > 2 && strcmp(argv[1], "-s") == 0)
   {
      strcpy(source, argv[2]);

      argc -= 2;
      argv += 2;

      if(strncasecmp(source, "gf", 2) == 0
      || strncasecmp(source, "gr", 2) == 0)
	 strcpy(type, "gftp");

      if(strncasecmp(source, "gp", 2) == 0)
	 strcpy(type, "gpfs");

      if(strncasecmp(source, "nvo", 3) == 0
      || strncasecmp(source, "uri", 3) == 0)
	 strcpy(type, "uri");
   }

   if(argc < 7)
   {
      printf("[struct stat=\"ERROR\", msg=\"Usage: %s survey band object|location width height outfile (object/location must be a single argument string)\"]\n", argv[0]);
      exit(0);
   }

   strcpy(server, "irsa.ipac.caltech.edu");

   port = 80;

   strcpy(base, "/cgi-bin/ImgList/nph-imglist?");

   survey    = url_encode(argv[1]);
   band      = url_encode(argv[2]);
   locstr    = url_encode(argv[3]);
   widthstr  = url_encode(argv[4]);
   heightstr = url_encode(argv[5]);

   sprintf(constraint, "survey=%s&band=%s&location=%s&width=%s&height=%s&mode=%s",
      survey, band, locstr, widthstr, heightstr, type);

   fout = fopen(argv[6], "w+");

   if(fout == (FILE *)NULL)
   {
      printf("[struct stat=\"ERROR\", msg=\"Can't open output file %s\"]\n", 
	 argv[6]);
      exit(0);
   }


   /* Connect to the port on the host we want */

   socket = tcp_connect(server, port);
  

   /* Send a request for the file we want */

   sprintf(request, "GET %s%s HTTP/1.0\r\nHOST: %s:%d\r\n\r\n",
      base, constraint, server, port);

   if(debug)
   {
      printf("DEBUG> request = [%s]\n", request);
      fflush(stdout);
   }

   send(socket, request, strlen(request), 0);


   /* And read all the lines coming back */

   count = 0;

   while(1)
   {
      /* Read lines returning from service */

      if(readline (socket, line) == 0)
	 break;

      if(debug)
      {
	 printf("DEBUG> return; [%s]\n", line);
	 fflush(stdout);
      }

      if(strncmp(line, "ERROR: ", 7) == 0)
      {
	 if(line[strlen(line)-1] == '\n')
	    line[strlen(line)-1]  = '\0';

	 printf("[struct stat=\"ERROR\", msg=\"%s\"]\n", line+7);
	 exit(0);
      }
      else
      {
	 fprintf(fout, "%s", line);
	 fflush(fout);

	 if(line[0] != '|'
	 && line[0] != '\\')
	    ++count;
      }
   }
      
   fclose(fout);

   printf("[struct stat=\"OK\", count=\"%d\"]\n", count);
   fflush(stdout);

   exit(0);
}




/***********************************************/
/* This is the basic "make a connection" stuff */
/***********************************************/

int tcp_connect(char *hostname, int port)
{
   int                 sock_fd;
   struct hostent     *host;
   struct sockaddr_in  sin;


   if((host = gethostbyname(hostname)) == NULL) 
   {
      printf("[struct stat=\"ERROR\", msg=\"Couldn't find host %s\"]\n", hostname);
      fflush(stdout);
      return(0);
   }

   if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
   {
      printf("[struct stat=\"ERROR\", msg=\"Couldn't create socket()\"]\n");
      fflush(stdout);
      return(0);
   }

   sin.sin_family = AF_INET;
   sin.sin_port = htons(port);
   bcopy(host->h_addr, &sin.sin_addr, host->h_length);

   if(connect(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
   {
      printf("[struct stat=\"ERROR\", msg=\"%s: connect failed.\"]\n", hostname);
      fflush(stdout);
      return(0);
   }

   return sock_fd;
}




/***************************************/
/* This routine reads a line at a time */
/* from a raw file descriptor          */
/***************************************/

int readline (int fd, char *line) 
{
   int n, rc = 0;
   char c ;

   for (n = 1 ; n < MAXLEN ; n++)
   {
      if ((rc == read (fd, &c, 1)) != 1)
      {
	 *line++ = c ;
	 if (c == '\n')
	    break ;
      }

      else if (rc == 0)
      {
	 if (n == 1)
	    return 0 ; /* EOF */
	 else
	    break ;    /* unexpected EOF */
      }
      else 
	 return -1 ;
   }

   *line = 0 ;
   return n ;
}




/**************************************/
/* This routine URL-encodes a string  */
/**************************************/

static unsigned char hexchars[] = "0123456789ABCDEF";

char *url_encode(char *s)
{
   int      len;
   register int i, j;
   unsigned char *str;

   len = strlen(s);

   str = (unsigned char *) malloc(3 * strlen(s) + 1);

   j = 0;

   for (i=0; i<len; ++i)
   {
      str[j] = (unsigned char) s[i];

      if (str[j] == ' ')
      {
         str[j] = '+';
      }
      else if ((str[j] < '0' && str[j] != '-' && str[j] != '.') ||
               (str[j] < 'A' && str[j] > '9')                   ||
               (str[j] > 'Z' && str[j] < 'a' && str[j] != '_')  ||
               (str[j] > 'z'))
      {
         str[j++] = '%';

         str[j++] = hexchars[(unsigned char) s[i] >> 4];

         str[j]   = hexchars[(unsigned char) s[i] & 15];
      }

      ++j;
   }

   str[j] = '\0';

   return ((char *) str);
}
