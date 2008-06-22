#include <stdio.h>
#include <stdlib.h>
#include <bqp_data.h>

#define NUMNODES 8
#define NUMWALLS 10

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

main() {
  int nodes[8] = {1, 2, 4, 8, 16, 32, 64, 96};
  int walltimes[10] = {30, 300, 600, 1800, 3600, 7200, 14400, 28800, 43200, 86400};
  int i, j, k, rnprod, numres, rc, r, numrnprods, *rnprods, prevrnprod;
  double p, retp;
  bqp_res *theres;
  char *mach, *queue;
  FILE *MFH, *DFH;

  numrnprods = NUMNODES * NUMWALLS;
  rnprods = malloc(sizeof(int) * numrnprods);

  k=0;
  for (i=0; i<NUMNODES; i++) {
    for (j=0; j<NUMWALLS; j++) {
      rnprods[k] = nodes[i] * walltimes[j];
      k++;
    }
  }
  qsort(rnprods, numrnprods, sizeof(int), intcomp);
  
  rc = bqp_get_resources(&theres, &numres);
  if (rc <= 0) {
    printf("ERROR: bqp_get_resources %d\n", rc);
    exit(1);
  }
  
  
  MFH = fopen("m.idx", "w");
  DFH = fopen("3d.dat", "w");

  k=0;
  for (r=0; r<numres; r++) {
    //    printf("%s\n", theres[i].resource);
    mach = theres[r].resource;
    queue = theres[r].queues[theres[r].default_qidx];

    if (strcmp("fnalcondor", mach) && strcmp("uctg-spruce", mach) && strcmp("iuteragrid", mach)) {
      fprintf(MFH, "%d %s %s\n", k, mach, queue);
      prevrnprod = -1;
      for (i=0; i<numrnprods; i++) {
	if (prevrnprod != rnprods[i]) {
	  rc = bqp_get_inverted(mach, queue, 1, rnprods[i], 14400, 0, &retp);
	  fprintf (DFH, "%d %d %f\n", k, rnprods[i], retp);
	}
	prevrnprod = rnprods[i];
      }
      k++;
    }
  }
  fclose(MFH);
  fclose(DFH);
}
