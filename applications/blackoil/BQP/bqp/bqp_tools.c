#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#include <cnet.h>
#include <common.h>
#include <bqp_data.h>
#include <bqp_tools.h>

void *varq_reserve_func(void *);

enum{VARQ_STATUS_NULL, VARQ_STATUS_IDLE, VARQ_STATUS_ERROR, VARQ_STATUS_DONE};

typedef struct varq_reservation_t {
  int id;
  int status;
} varq_reservation;



int varq_reserve(char *cluster_name, char *queue, int nodes, int reqtime, int start_deadline, double min_success_prob) {
  }

void *varq_reserve_func(void *in) {  
  return(NULL);
}

int varq_reserve_block(char *cluster_name, char *queue, int nodes, int reqtime, int start_deadline, double min_success_prob, int *outwait, int *outrtime, double *outprob) {
  int refresh = 2, minwait = 60;
  int new_deadline, orig_deadline, owait, ortime;
  double oprob;
  int rc, done=0;

  orig_deadline = time(NULL) + start_deadline;
  new_deadline = orig_deadline - time(NULL);
  while(!done) {
    rc = varq_request(cluster_name, queue, nodes, reqtime, new_deadline, min_success_prob, &owait, &ortime, &oprob);
    if (rc < 0) {
      return(1);
    }
    *outwait = owait;
    *outrtime = ortime;
    *outprob = oprob;
    //printf("%d %d %d %f\n", new_deadline, *outwait, *outrtime, *outprob);
    sleep(refresh);
    new_deadline = orig_deadline - time(NULL);
    if (new_deadline < minwait) {
      *outwait = 0;
      *outrtime = reqtime + (orig_deadline - time(NULL));
      return(1);
    }
  }


}

int varq_request(char *cluster_name, char *queue, int nodes, int reqtime, int start_deadline, double min_success_prob, int *outwait, int *outrtime, double *outprob) {
  int past=0, wait, realrtime, idx, realidx, rc;
  double newprob, lastprob, realprob;
  
  idx=realidx=0;
  realprob = 0.0;
  wait = realrtime = past = 0;

  rc = bqp_get_inverted(cluster_name, queue, nodes, reqtime+start_deadline, start_deadline, 0.0, &newprob);

  lastprob = newprob;
  if (newprob >= min_success_prob) {
    wait = past;
    realrtime = reqtime + (start_deadline - past);
    realprob = newprob;
    idx=0;
  }


  while(start_deadline - past > 0) {
    if (newprob >= min_success_prob) {
      wait = past;
      realrtime = reqtime + (start_deadline - past);
      realprob = newprob;
      realidx = idx;
    }

    if (lastprob < min_success_prob && newprob >= min_success_prob) {
      wait = past;
      realrtime = reqtime + (start_deadline - past);
      realprob = newprob;
      realidx = idx;
    } else if (lastprob >= min_success_prob && newprob < min_success_prob) {
      wait = past - 30;
      realrtime = reqtime + (start_deadline - (past - 30));
      realprob = lastprob;
      realidx = idx-1;
    }
    lastprob = newprob;
    past += 30;

    rc = bqp_get_inverted(cluster_name, queue, nodes, reqtime + (start_deadline-past), start_deadline-past, 0.0, &newprob);

    idx++;
  }

  *outwait = wait;
  *outrtime = realrtime;
  *outprob = realprob;
  if (wait == 0 && realrtime == 0) {
    return(-10);
  }
  return(1);
}

