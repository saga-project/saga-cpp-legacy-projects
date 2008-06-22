#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bqp_data.h>

#define ARGS "q:m:n:N:r:R:d:D:p:P:s:"
char *usage = "varq_range -m <machine> -q <queue> -n <nodemin> -N <nodemax> -r <wallmin> -R <wallmax> -d <startdeadlinemin> -D <startdeadlinemax> -p <probmin> -P <probmax>\n";

main(int argc, char **argv) {
  int ns, ne, ws, we, ds, de, rc, wait, realrtime, steps;
  double ps, pe, realprob, prob, probstep;
  char *mach, *queue;
  int nodes, walltime, startdeadline, c;
  int nodestep, wallstep, deadlinestep;

  steps = 10;
  ns = ne = 1;
  ws = we = 3600;
  ds = de = 14400;
  ps = pe = .5;

  mach = NULL;
  queue = NULL;

  while((c = getopt(argc, argv, ARGS)) != EOF) {
    switch(c) {
    case 'm':
      mach = strdup(optarg);
      break;
    case 'q':
      queue = strdup(optarg);
      break;
    case 'n':
      ns = atoi(optarg);
      break;
    case 'N':
      ne = atoi(optarg);
      break;
    case 'r':
      ws = atoi(optarg);
      break;
    case 'R':
      we = atoi(optarg);
      break;
    case 'd':
      ds = atoi(optarg);
      break;
    case 'D':
      de = atoi(optarg);
      break;
    case 'p':
      ps = atof(optarg);
      break;
    case 'P':
      pe = atof(optarg);
      break;
    case 's':
      steps = atoi(optarg);
      break;
    default:
      break;
    }
  }

  if (mach == NULL || queue == NULL) {
    printf("ERROR: must specify, at least, a machine and queue (-m, -q)\n");
    printf("USAGE: %s\n", usage);
    exit(1);
  } else if (de < ds || ne < ns || we < ws || pe < ps) {
    printf("ERROR: min must be less than max for all min/max pairs\n");
    printf("USAGE: %s\n", usage);
    exit(1);
  }

  nodestep = (ne - ns) / steps;
  if (nodestep <= 0) {
    nodestep=1;
  }

  wallstep = (we - ws) / steps;
  if (wallstep <= 0) {
    wallstep = 1;
  }

  deadlinestep = (de - ds) / steps;
  if (deadlinestep <= 0) {
    deadlinestep = 1;
  }

  probstep = (pe - ps) / (double)steps;
  if (probstep <= 0.0) {
    probstep = 1.0;
  }
  
  printf("%-8s %4s %8s %8s %8s %8s %8s %8s\n", "#action", "node", "wall", "deadline", "minprob", "towait", "touse", "realprob");
  for (nodes=ns; nodes<=ne; nodes+=nodestep) {
    for (walltime=ws; walltime<=we; walltime+=wallstep) {
      for (startdeadline=ds; startdeadline<=de; startdeadline+=deadlinestep) {
	for (prob=ps; prob<=pe; prob+=probstep) {
	  rc = varq_request(mach, queue, nodes, walltime, startdeadline, prob, &wait, &realrtime, &realprob);
	  if (rc < 0) {
	    printf("%-8s %4d %8d %8d %8.2f %8d %8d %8.6f\n", "FAIL", nodes, walltime, startdeadline, prob, 0, 0, 0.0);
	  } else {
	    printf("%-8s %4d %8d %8d %8.2f %8d %8d %8.6f\n", "SUCCESS", nodes, walltime, startdeadline, prob, wait, realrtime, realprob);
	  }
	}
      }
    }
  }

}

int intcomp(const void *ina, const void *inb) {
  int *a, *b;
  a = (int *)ina;
  b = (int *)inb;
  
  if (*a < *b) {
    return(-1);
  } else if (*a > *b) {
    return(1);
  }
  return(0);
}

