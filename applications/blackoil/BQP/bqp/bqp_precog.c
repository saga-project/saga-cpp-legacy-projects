#include <stdio.h>
#include <stdlib.h>
#include <bqp_vgrads_api.h>

main(int argc, char **argv) {
  int rc, id, nnodes, nreqtime, ndeadline, wait, realrtime;
  double ret, nprob, realprob;
  char *nmach, *nqueue;

  if (argc < 7) {
    printf("USAGE: %s mach queue nodes rtime deadline reqprob\n", argv[0]);
    exit(1);
  }

  nmach = argv[1];
  nqueue = argv[2];
  nnodes = atoi(argv[3]);
  nreqtime = atoi(argv[4]);
  ndeadline = atoi(argv[5]);
  nprob = atof(argv[6]);
  
  bqp_init();

  //  printf("CALLING WITH %s %d %d %d %f\n", nmach, nnodes, nreqtime, ndeadline, nprob);
  //  ret = bqp_query(nmach, nnodes, nreqtime, ndeadline);
  ret = nprob;
  if (ret >= nprob) {

    rc = bqp_precog(nmach, nnodes, nreqtime, ndeadline, nprob, &wait, &realrtime, &realprob);
    if (rc < 0) {
      printf("FAIL 0\n");
      exit(1);
    } else {
      printf("BIND %d %s %s %d %d %d %f\n", wait, nmach, nqueue, nnodes, realrtime, ndeadline-wait, realprob);
      //"BIND $elapsed $mach $queue $nodes $realrtime $realdeadline $prob\n";
    }
  } else {
    printf("FAIL 0\n");
    exit(1);
  }
  exit(0);
}
