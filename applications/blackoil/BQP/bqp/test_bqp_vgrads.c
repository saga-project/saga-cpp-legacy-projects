#include <stdio.h>
#include <stdlib.h>
#include <bqp_vgrads_api.h>


main() {
  int rc, id, nnodes, nreqtime, ndeadline, i, deadlinets;
  double ret, nprob;
  int pid;

  pid = fork();
  if (pid) {
    deadlinets = time(NULL) + 120;
    //  rc = bqp_reserve_v2("mayhem", 2, 1200, deadlinets - time(NULL), .2);
    rc = bqp_reserve_v2("mayhem", 2, 1920, deadlinets - time(NULL), .2);
    printf("RC: %d\n", rc);
    rc = 0;
    while(!rc) {
      //    rc = bqp_reserve_check_v2("mayhem", 2, 1200, deadlinets - time(NULL), .2);
      rc = bqp_reserve_check_v2("mayhem", 2, 1920, deadlinets - time(NULL), .2);
      sleep(1);
    }
  } else {
    exit(0);
    deadlinets = time(NULL) + 180;
    //  rc = bqp_reserve_v2("mayhem", 2, 1200, deadlinets - time(NULL), .2);
    rc = bqp_reserve_v2("sdscteragrid", 2, 1920, deadlinets - time(NULL), .2);
    printf("RC: %d\n", rc);
    rc = 0;
    while(!rc) {
      //    rc = bqp_reserve_check_v2("mayhem", 2, 1200, deadlinets - time(NULL), .2);
      rc = bqp_reserve_check_v2("sdscteragrid", 2, 1920, deadlinets - time(NULL), .2);
      sleep(1);
    }
  }
  exit(0);


  bqp_init();

  ret = bqp_query("mercury", 4, 3600, 43200);

  printf("RET: %f\n", ret);

  rc = bqp_alternative("ucteragrid", -4, 3600, 43200, .75, &nnodes, &nreqtime, &ndeadline, &nprob);
  printf("ALT 1: %d %d %d %d %f\n", rc, nnodes, nreqtime, ndeadline, nprob);

  rc = bqp_alternative("ucteragrid", 4, -3600, 43200, .75, &nnodes, &nreqtime, &ndeadline, &nprob);
  printf("ALT 2: %d %d %d %d %f\n", rc, nnodes, nreqtime, ndeadline, nprob);

  rc = bqp_alternative("ucteragrid", 4, 3600, -43200, .75, &nnodes, &nreqtime, &ndeadline, &nprob);
  printf("ALT 3: %d %d %d %d %f\n", rc, nnodes, nreqtime, ndeadline, nprob);

  rc = bqp_alternative("ucteragrid", -4, -3600, -43200, .75, &nnodes, &nreqtime, &ndeadline, &nprob);
  printf("ALT 4: %d %d %d %d %f\n", rc, nnodes, nreqtime, ndeadline, nprob);


  for (i=0; i<10; i++) {
    pid = fork();
    if (pid) {
    } else {
    rc = bqp_reserve("ncsateragrid", 8, 1200, 10000, .30);
    if (rc == 0) {
      printf("DOH cannot reserve\n");
      exit(0);
    } else if (rc < 0) {
      printf("DOH??? %d\n", rc);
      exit(0);
    }
    id = rc;
    //  printf("ID: %d\n", id);
    rc = bqp_reserve_check(id);
    while(!rc) {
      printf("checky %d\n", rc);
      printf("sleeping...");
      fflush(stdout);
      sleep(1);
      printf("done\n");
      fflush(stdout);
      rc = bqp_reserve_check(id);
    }
    printf("RC: %d\n", rc);
    
    bqp_exit(id);
    }
  }
}
