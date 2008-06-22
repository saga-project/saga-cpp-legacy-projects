#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>

#include <bqp_vgrads_api.h>

#define NUMCLUSTERS 10
#define NUMSLOTS 2048

int pipes[NUMSLOTS][2];
int pids[NUMSLOTS];

void bqp_init() {
  int i;
  for (i=0; i<NUMSLOTS; i++) {
    pipes[i][0] = -1;
    pids[i] = 0;
  }
}

void bqp_exit(int id) {
  
  if (pids[id] == 0) {
  } else {
    kill(pids[id], 9);
    wait(pids[id]);
    close(pipes[id][0]);
    close(pipes[id][1]);
  }

}

int vgrads_cluster_map(char *cluster_name, char *out_cluster_name, char *out_queue) {
  char extclusters[NUMCLUSTERS][32] = {"ncsateragrid", "ucteragrid", "sdscteragrid", "cnsidell", "mercury", "dante", "iuteragrid", "mayhem", "ada", "ornlteragrid"};
  char intclusters[NUMCLUSTERS][32] = {"ncsateragrid", "ucteragrid", "sdscteragrid", "cnsidell", "ncsateragrid", "dante", "iuteragrid", "mayhem", "ada", "ornlteragrid"};
  char queues[NUMCLUSTERS][32] = {"dque", "dque", "dque", "ALL", "dque", "dque", "dque", "dque", "compute", "dque"};

  char themach[32], thequeue[32];
  int i, j, done, cid, rc;

  bzero(themach, 32);
  bzero(thequeue, 32);

  done=0;
  cid = -1;
  for (i=0; i<NUMCLUSTERS && !done; i++) {
    if  (!strncmp(cluster_name, extclusters[i], 32)) {
      cid = i;
      done++;
    }
  }

  if (cid < 0) {
    bqp_res *theres;
    int numres;

    theres = NULL;
    rc = bqp_get_resources(&theres, &numres);
    if (rc <= 0) {
      if (theres != NULL) {
	free(theres);
      }
      return(-30.0);
    }

    for (i=0; i<numres; i++) {
      if (!strncmp(theres[i].resource, cluster_name, 32)) {
	strncpy(themach, theres[i].resource, 32);
	strncpy(thequeue, theres[i].queues[theres[i].default_qidx], 32);
	cid=1;
      }
    }
    free(theres);
    if (cid < 0) {
      return(-30.0);
    }
  } else {
    strncpy(themach, intclusters[cid], 32);
    strncpy(thequeue, queues[cid], 32);
  }

  strncpy(out_cluster_name, themach, 32);
  strncpy(out_queue, thequeue, 32);
  return(1);
}

double bqp_query(char *cluster_name, int nodes, int reqtime, unsigned int deadline) {
  int rc, cid=0, i, done=0;
  double result;
  char themach[32], thequeue[32];

  if (nodes < 1) {
    return(-10.0);
  }

  if (reqtime < 1) {
    return(-20.0);
  }
  
  if (deadline < 1) {
    return(-40.0);
  }

  rc = vgrads_cluster_map(cluster_name, themach, thequeue);
  if (rc < 0) {
    return((double)rc);
  }

  if (0) {
    char hfile[2048],*hbuf, *strptr, line[128], *chomp;
    bzero(hfile, 2048);
    
    sprintf(hfile, "/~nurmi/sc06reservations");
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc < 0) {
    } else {
      strptr = hbuf;
      while(sscanf(strptr, "%s", line) != EOF) {
        chomp = strchr(strptr, '\n');
        if (chomp != NULL) {
          *chomp = '\0';
	  //printf("|%s| |%s|\n", intclusters[cid], strptr);
          if (!strcmp(themach, strptr)) {
            return(1.0);
          }
          strptr = chomp;
          strptr++;
        }
      }
    }
  }

  result = 0.0;
  //  printf("Q: %s %s %d %d %d\n", themach, thequeue, nodes, reqtime, deadline);
  rc = bqp_get_inverted(themach, thequeue, (int)nodes, (int)reqtime, (int)deadline, 0.0, &result);
  //  printf("%d %f\n", rc, result);
  //  printf("WTF: %f\n", result);
  if (rc < 0) {
    return((double)rc);
  }

  return(result);
}

int bqp_reserve_v2(char *cluster_name, int nodes, int reqtime, int deadlinets, double prob) {
  int rc, realwait, realrtime, deadline;
  double realprob;
  char themach[32], thequeue[32];
  
  //  deadline = deadlinets - time(NULL);
  deadline = deadlinets;
 
  //  printf("FOO %s %s %s\n", cluster_name, themach, thequeue);
  rc = bqp_precog(cluster_name, nodes, reqtime, deadline, prob, &realwait, &realrtime, &realprob);


  if (rc < 0) {
    //    printf("cant reserve: %d %d %f\n", realwait, realrtime, realprob);
    return(-10);
  } else {
    //    printf("can reserve %d %d %f\n", realwait, realrtime, realprob);
  }
  return(1);
}

int bqp_reserve_check_v2(char *cluster_name, int nodes, int reqtime, int deadlinets, double prob) {
  int deadline, rc, realwait, realrtime;
  double realprob;
  
  //  deadline = deadlinets - time(NULL);
  deadline = deadlinets;
  rc = bqp_precog(cluster_name, nodes, reqtime, deadline, prob, &realwait, &realrtime, &realprob);
  if ((rc < 0 || realwait <= 30) || realprob == 1.0) {
    if (realprob == 1.0) {
      printf("reserve mode, dont wait, reserve now!\n");
    } else {
      printf("cant wait: %d %d %f, %d %d, time to submit\n", realwait, realrtime, realprob, time(NULL), deadlinets);
    }
    return(1);
  } else {
    printf("can reserve in %d for %d p=%f, %d %d\n", realwait, realrtime, realprob, time(NULL), deadlinets);
  }
  return(0);
}

int bqp_reserve(char *cluster_name, int nodes, int reqtime, int deadline, double prob) {
  // set up a pipe, fork, do exp6, woot
  int rc, id, i, done=0, pid;
  time_t toolate, startexper;
  double currprob;

  startexper = time(NULL);
  toolate = startexper + (time_t)deadline;

  for (i=1; i<NUMSLOTS && !done; i++) {
    if (pipes[i][0] == -1) {
      id = i;
      pipes[i][0] = 0;
      done++;
    }
  }

  if (!done) {
    return(-10);
  }

  rc = pipe(pipes[id]);
  if (rc < 0) {
    perror("pipe");
    return(-20);
  }
  
  pid = fork();
  if (pid) {
    // parent
    pids[id] = pid;
  } else {
    // child
    int done=0, gap, mode=0;
    char buf[3];
    time_t now;

    srand(time(NULL));
    
    now = time(NULL);
    gap = toolate - now;
    
    currprob = bqp_query(cluster_name, nodes, reqtime, (int)((time_t)toolate - (time_t)now));
    printf("FIRST %d %d %f %f\n", gap+reqtime, toolate - now, currprob, prob);
    if (prob > currprob) {
      bzero(buf, 3);
      sprintf(buf, "FF");
      write(pipes[id][1], buf, 3);
      done++;
    } else {
      bzero(buf, 3);
      sprintf(buf, "NO");
      write(pipes[id][1], buf, 3);
      done=0;
    }

    while(!done) {
      now = time(NULL)+60;
      gap = (int)(toolate - now);

      currprob = bqp_query(cluster_name, nodes, gap+reqtime, (int)((time_t)toolate - (time_t)now));
      printf("%d %d %f\n", gap+reqtime, toolate - now, currprob);
      fflush(stdout);
      //      if ((rand() % 10) == 4 || time(NULL) > toolate) {
      if (currprob >= prob || mode == 1) {
	mode = 1;
	if (currprob < prob) {
	  done++;
	} else {
	  sleep(1);
	}
      } else {
	sleep(1);
      }
    }

    if(time(NULL) > toolate) {
      //      printf("FFFFF\n");
      sprintf(buf, "FF");
      write(pipes[id][1], buf, 3);
    } else {
      sprintf(buf, "GO");
      write(pipes[id][1], buf, 3);
    }
    exit(0);
  }
  

  return(id);
}

int bqp_reserve_check(int id) {
  char buf[3];
  fd_set rfds;
  struct timeval timeout;
  int rc;

  // use the pipe luke!
  bzero(buf, 3);
  
  if (pipes[id] == NULL) {
    return(-10);
  }

  FD_ZERO(&rfds);
  FD_SET(pipes[id][0], &rfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = 1000;
    
  //  printf("reading...");
  fflush(stdout);
  if (select(pipes[id][0]+1, &rfds, NULL, NULL, &timeout) <= 0) {
    //    printf("select thinks nuthin\n");
    return(0);
  }

  rc = read(pipes[id][0], buf, 3);
  //  printf("RBUF: %s\n", buf);
  if (!strncmp(buf, "NO", 3)) {
    return(0);
  } else if (!strncmp(buf, "GO", 3)) {
    close(pipes[id][0]);
    close(pipes[id][1]);
    return(1);
  } else if (!strncmp(buf, "FF", 3)) {
    close(pipes[id][0]);
    close(pipes[id][1]);
    return(-1);
  } else {
    return(-1);
  }
}

int bqp_alternative(char *cluster_name, int innodes, int inreqtime, int indeadline, double prob, int *alt_nodes, int *alt_reqtime, int *alt_deadline, double *alt_prob) {
  double prc, nodes, reqtime, deadline;
  int done, nodefudge, reqtimefudge, deadlinefudge;
  int county;

  nodefudge = reqtimefudge = deadlinefudge = 0;
  county = 0;

  nodes = (double)innodes;
  reqtime = (double)inreqtime;
  deadline = (double)indeadline;

  if (nodes < 0.0) {
    nodes *= -1.0;
    nodefudge = 1;
  } 

  if (reqtime < 0.0) {
    reqtime *= -1.0;
    reqtimefudge = 1;
  } 

  if (deadline < 0.0) {
    deadline *= -1.0;
    deadlinefudge = 1;
  } 
  
  done=0;
  while(!done) {
    //    printf("%f %f %f\n", nodes, reqtime, deadline);
    *alt_nodes = (int)nodes;
    *alt_reqtime = (int)reqtime;
    *alt_deadline = (int)deadline;
    prc = bqp_query(cluster_name, (int)nodes, (int)reqtime, (int)deadline);
    *alt_prob = prc;

    if (prc <= 0.0) {
      // crap!
      printf("NOOO %f\n", prc);
      
      return(-10);
    } else if (prc >= prob) {
      // yay!
      return(1);
    } else {
      // try agin
      if (!nodefudge && !reqtimefudge && !deadlinefudge) {
	// nocando
	return(-10);
      } else {
	if (nodefudge) {
	  nodes *= 0.99;
	  if (nodes < 1.0) {
	    nodes = 1.0;
	    nodefudge = 0;
	  }
	}
	if (reqtimefudge) {
	  reqtime *= 0.98;
	  if (reqtime < 60.0) {
	    reqtime = 60.0;
	    reqtimefudge = 0;
	  }
	}
	if (deadlinefudge) {
	  deadline *= 1.02;
	  if (deadline > 173000.0) {
	    deadline = 173000.0;
	    deadlinefudge = 0;
	  }
	}
      }
    }
    county++;
  }
}

int bqp_precog(char *cluster_name, int nodes, int reqtime, int deadline, double prob, int *outwait, int *outrtime, double *outprob) {
  int rc;
  char themach[32], thequeue[32];

  rc = vgrads_cluster_map(cluster_name, themach, thequeue);
  if (rc < 0) {
    return(rc);
  }
  
  rc = varq_request(themach, thequeue, nodes, reqtime, deadline, prob, outwait, outrtime, outprob);
  return(rc);

}

int bqp_precog_orig(char *cluster_name, int nodes, int reqtime, int deadline, double prob, int *outwait, int *outrtime, double *outprob) {
  int past=0, wait, realrtime, idx, realidx;
  double newprob, lastprob, realprob;
  
  idx=realidx=0;
  realprob = 0.0;
  wait = realrtime = past = 0;

  newprob = bqp_query(cluster_name, nodes, reqtime+deadline, deadline);
  lastprob = newprob;
  if (newprob >= prob) {
    wait = past;
    realrtime = reqtime + (deadline - past);
    realprob = newprob;
    idx=0;
  }


  while(deadline - past > 0) {
    if (newprob >= prob) {
      wait = past;
      realrtime = reqtime + (deadline - past);
      realprob = newprob;
      realidx = idx;
    }

    if (lastprob < prob && newprob >= prob) {
      wait = past;
      realrtime = reqtime + (deadline - past);
      realprob = newprob;
      realidx = idx;
      //      printf("WINNER %d %f\n", realidx, realprob);
    } else if (lastprob >= prob && newprob < prob) {
      wait = past - 30;
      realrtime = reqtime + (deadline - (past - 30));
      realprob = lastprob;
      realidx = idx-1;
      //      printf("WINNER %d %f\n", realidx, realprob);
    }
    lastprob = newprob;
    past += 30;
    //    printf("TEST %d %f %d\n", idx, newprob, realidx);
    newprob = bqp_query(cluster_name, nodes, reqtime+(deadline-past), deadline-past);
    idx++;
  }

  //  printf("WAIT: %d REALRTIME %d\n", wait, realrtime);
  //  printf("TOT: %d\n", idx);
  *outwait = wait;
  *outrtime = realrtime;
  *outprob = realprob;
  if (wait == 0 && realrtime == 0) {
    return(-10);
  }
  return(1);
}
