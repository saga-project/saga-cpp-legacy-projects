#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <bqp_data.h>
#include <common.h>

#define ARGS "ABq:n:r:hm:lQp:c:d:P:"
#ifdef SDSC
char *usage = "bqp -p <quantile> -c <confidence> ...\n\
\t\t-p quantile (default=0.95) specify prediction quantile (.95, .75, or .5)\n\
\t\t-c confidence (default=0.95) specify a confidence on the quantile (.99, .95, .90)\n\
\t\t-q queuename (optional) restrict information to given queue\n\
\t\t-n numnodes (optional) restrict info to given job node size\n\
\t\t-r req time (optional) restrict info to given job requested time\n\
\n\
bqp -l\tlist all machine/queue pairs supported\n\
\n\
bqp -q <queue> -n <nodes> -r <reqtime> -d <deadline>\n\
bqp -q <queue> -n <nodes> -r <reqtime> -d <deadline> -P <prob constraint>\n\
\n\
bqp -B -q <queue> -n <nodes> -r <reqtime>\tlist job choices with shortest predicted queue wait time\n";
//char *logfile = "/usr/local/apps/bqp/lib/bqpusage.log";
char *logfile = "/tmp/bqpusage.log";
FILE *FH;

#else
char *usage = "bqp -p <quantile> -c <confidence> ...\n\
\t\t-p quantile (default=0.95) specify prediction quantile (.95, .75, or .5)\n\
\t\t-c confidence (default=0.95) specify a confidence on the quantile (.99, .95, .90)\n\
\t\t-q queuename (optional) restrict information to given queue\n\
\t\t-n numnodes (optional) restrict info to given job node size\n\
\t\t-r req time (optional) restrict info to given job requested time\n\
\n\
bqp -l\tlist all machine/queue pairs supported\n\
\n\
bqp -m <mach> -q <queue> -n <nodes> -r <reqtime> -d <deadline>\n\
\n\
bqp -B -m <mach> -q <queue> -n <nodes> -r <reqtime>\tlist job choices with shortest predicted queue wait time\n";
#endif

int main(int argc, char **argv) {
  char *queue, *mach;
  int mode, c, rc, i, dolist, j,numres, done;
  double nodes, reqtime, deadline;
  int nodefudge=0, reqtimefudge=0, deadlinefudge=0, fudgecount=0;
  bqp_res *theres;
  double quantile, confidence, result, needprob=0.0;
  char *obuf;

  obuf = (char *)malloc(sizeof(char)*32768*5);
  bzero(obuf, sizeof(char)*32768*5);
#ifdef SDSC
  FH = fopen(logfile, "a+");
  if (FH == NULL) {
    printf("ERROR: cannot open log file %s\n", logfile);
    exit(1);
  }
  fprintf(FH, "LOGENT %u - %d - %d - ", time(NULL), getuid(), getgid());
  for(i=0; i<argc; i++) {
    fprintf(FH, "%s ", argv[i]);
  }
  fprintf(FH, "\n");
#endif

  quantile = 0.95;
  confidence = 0.05;
#ifdef SDSC
  mach = strdup("sdscteragrid");
#else
  mach = NULL;
#endif
  queue = NULL;
  nodes = 0.0;
  reqtime = 0.0;
  mode = PRINTALL;
  dolist = 0;

  while((c = getopt(argc, argv, ARGS)) != EOF) {
    switch(c) {
    case 'q':
      queue = strdup(optarg);
      break;
    case 'm':
      mach = strdup(optarg);
      break;
    case 'n':
      nodes = atof(optarg);
      if (nodes < 0.0) {
	nodefudge = 1;
	nodes *= -1.0;
      }
      break;
    case 'r':
      reqtime = atof(optarg);
      if (reqtime < 0.0) {
	reqtime *= -1.0;
	reqtimefudge = 1;
      }
      break;
    case 'A':
      mode = PRINTALL;
      break;
    case 'd':
      mode = INVERTED;
      deadline = atoi(optarg);
      if (deadline < 0) {
	deadline *= -1;
	deadlinefudge = 1;
      }
      break;
    case 'l':
      dolist = 1;
      break;
    case 'B':
      mode = WINNERS;
      break;
    case 'Q':
      mode = PRINTONE;
      break;
    case 'p':
      quantile = atof(optarg);
      break;
    case 'c':
      confidence = 1 - atof(optarg);
      break;
    case 'P':
      needprob = atof(optarg);
      break;
    default:
      printf("%s", usage);
      exit(0);
      break;
    }
  }

  if (dolist) {
    rc = bqp_get_resources(&theres, &numres);
    if (rc <= 0) {
      exit(1);
    }

    for (i=0; i<numres; i++) {
      printf("%s\n", theres[i].resource);
      for (j=0; j<theres[i].numqueues; j++) {
        printf("\t%s\n", theres[i].queues[j]);
      }
    }
    exit(0);
  }

  if (mode == WINNERS) {
    winner winners[2048];
    int numwinners;
    
    rc = bqp_print_all_predictions_internal(0, mach, queue, (int)nodes, (int)reqtime,quantile,confidence, winners, &numwinners);
    if (rc <= 0) {
      exit(1);
    }
    if (numwinners > 10) {
      numwinners = 10;
    }
    for (i=0; i<numwinners; i++) {
#ifdef SDSC
      {
	int minutes;
	char tstr[2];
	minutes = (int)winners[i].num / 60;
	strcpy(tstr, "m");
	if (minutes >= 60 && minutes <= 1440) {
	  minutes /= 60;
	  strcpy(tstr, "h");
	} else if (minutes > 1440) {
	  minutes = minutes / 60 / 24;
	  strcpy(tstr,"d");
	}
	sprintf(obuf, "%s%s   prediction:%d%s\n", obuf,winners[i].str, minutes, tstr);
      }
#else
      sprintf(obuf,"%s%s - %f\n", obuf,winners[i].str, winners[i].num);
#endif
    }
  } else if (mode != INVERTED) {
    rc = bqp_print_all_predictions(1, mach, queue, (int)nodes, (int)reqtime,quantile,confidence);
    if (rc <= 0) {
      exit(1);
    }
  } else if (mode == INVERTED) {
    done=0;
    while(!done) {
      rc = bqp_get_inverted(mach, queue, (int)nodes, (int)reqtime, (int)deadline, needprob, &result);
      if (rc < 0) {
	sprintf(obuf, "%sp=%f\n",obuf,0.0);
	done++;
      } else if (result >= needprob) {
	sprintf(obuf,"%sp=%f\n", obuf,result);
	if (needprob > 0.0) {
	  sprintf(obuf, "%snodes=%d\n", obuf, (int)nodes);
	  sprintf(obuf, "%srtime=%d\n", obuf, (int)reqtime);
	  sprintf(obuf, "%sdeadline=%d\n", obuf, (int)deadline);
	}
	done=1;
      } else {
	if (!nodefudge && !reqtimefudge && !deadlinefudge) {
	  sprintf(obuf,"%scannot satisfy needprob, need to specify job dimension flexibility\n",obuf);
	  done++;
	} else {
	  if (fudgecount % 3 == 0) {
	    if (reqtimefudge) {
	      reqtime *= 0.98;
	    }
	  }
	  if (fudgecount % 3 == 1) {
	    if (deadlinefudge) {
	      deadline *= 1.02;
	    }
	  }
	  if (fudgecount % 3 == 2) {
	    if (nodefudge && nodes > 1.0) {
	      nodes = nodes *= 0.99;
	    }
	  }
	  fudgecount++;
	  //	  printf("%f %f %f %f\n", nodes, reqtime, deadline, result);
	  if (nodes <= 1.0) {
	    nodes = 1.0;
	    nodefudge = 0;
	  }
	  if (reqtime <= 60.0) {
	    reqtime = 60.0;
	    reqtimefudge = 0;
	  }
	  if (deadline >= 172800) {
	    deadline = 172800.0;
	    deadlinefudge = 0;
	  }
	  if (!nodefudge && !deadlinefudge && !reqtimefudge) {
	    sprintf(obuf,"%scannot satisfy needprob, need to specify job dimension flexibility\n",obuf);
	    done++;
	  }
	}
      }
      

    }
  }
  
#ifdef SDSC
  fprintf(FH, "%s", obuf);
  fprintf(FH, "****************************************************\n");
  fclose(FH);
#endif
  printf("%s", obuf);
  free(obuf);
  exit(0);
}
