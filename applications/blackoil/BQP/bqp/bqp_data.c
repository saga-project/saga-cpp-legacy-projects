#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#include <bqp_data.h>
#include <cnet.h>
#include <common.h>

#define TSIZE 1024

//#define HTTPCACHE 1

char *hcache[2048];
char *hcachefiles[2048];
int hcacheinit = 0;
int hcacheinvalidate = 0;

#ifdef SDSC
extern FILE *FH;
#endif

int midx=0;

void *Malloc(size_t in) {
  return(malloc(in));
}

void Free(void *in) {
  return(free(in));
}

char *Strdup(const char *in) {
  return(strdup(in));
}

int sort_by_num_inv(const void *a, const void *b) {
  winner *ina, *inb;

  ina = (winner *)a;
  inb = (winner *)b;

  if (ina->num > inb->num) {
    return(-1);
  } else if (ina->num < inb->num) {
    return(1);
  } else {
    return(0);
  }
}

int sort_by_num(const void *a, const void *b) {
  winner *ina, *inb;

  ina = (winner *)a;
  inb = (winner *)b;

  if (ina->num > inb->num) {
    return(1);
  } else if (ina->num < inb->num) {
    return(-1);
  } else {
    return(0);
  }
}

int get_http_file(char *hostname, char *file, char **result) {
  int sd, rc, size, i, first;
  char *buf, cbuf;
  char *uberbuf;

  if(HTTPCACHE) {
    //  printf("IN THE CACHE CODE %s\n", file);
    if (hcacheinit == 0 || (time(NULL) >= hcacheinvalidate)) {
      //    printf("invalidating cache\n");
      if (hcacheinit == 0) {
	bzero(hcache, sizeof(char *)*2048);
      } else {
	for (i=0; i<2048; i++) {
	  if (hcache[i] != NULL) {
	    free(hcache[i]);
	  }
	  if (hcachefiles[i] != NULL) {
	    free(hcachefiles[i]);
	  }
	}
	bzero(hcache, sizeof(char *)*2048);
      }
      hcacheinit = 1;
      hcacheinvalidate = time(NULL) + 120;
    }
    for (i=0; i<2048; i++) {
      if (hcache[i] != NULL) {
	if (!strcmp(file, hcachefiles[i])) {
	  //	printf("hit the cache!\n");
	  *result = strdup(hcache[i]);
	  //	  printf("CHIT: %s\n", hcachefiles[i]);
	  return(1);
	}
      }
    }
  }

  *result = NULL;
  rc = cnet_connect(HTTPHOST, 80, &sd, 10);
  //  printf("connect %d\n", rc);
  if (rc < 0) {
    fprintf(stderr, "ERROR: cnet_connect()\n");
    return(-1);
  }

  buf = (char *)malloc(1024);
  snprintf(buf, 1024, "GET %s HTTP/1.0\n\n", file);

  rc = cnet_sendbuf(sd, buf, strlen(buf), 10);
  if (rc < 0) {
    fprintf(stderr, "ERROR: cnet_sendbuf() %d\n", rc);
    return(-1);
  }
  free(buf);

  uberbuf = NULL;
  size = 0;
  buf = (char *)malloc(sizeof(char) * TSIZE+2);
    //  rc = cnet_recvbuf(sd, &cbuf, 1, 10);
  bzero(buf, TSIZE+2);
  rc = cnet_recvbuf(sd, buf, TSIZE, 10);
  if (rc < 0) {
    fprintf(stderr, "ERROR cnet_recvbuf() %d\n", rc);
    return(-1);
  }

  while(rc > 0) {

    //    printf("%c %d\n", cbuf, cbuf);
    //    printf("%s", buf);
    size += rc;
    uberbuf = (char *)realloc(uberbuf, size);
    //    memcpy(uberbuf + (size - rc), &cbuf, rc);
    memcpy(uberbuf + (size - rc), buf, rc);
    //    rc = cnet_recvbuf(sd, &cbuf, 1, 10);
    bzero(buf, TSIZE+2);
    rc = cnet_recvbuf(sd, buf, TSIZE, 10);
    if (rc < 0) {
      free(buf);
      free(uberbuf);
      return(-1);
    }
    //    printf("|%s|%d\n", buf, rc);
  }
  //  printf("\n-------------------------- %d\n", size);
  close(sd);
  //  printf("UB: %s\n", uberbuf);
  free(buf);

  first=0;
  for (i=0; i<size; i++) {
    if (first == 4) {
      *result = (char *)malloc(size - i + 1);
      bzero(*result, size-i + 1);
      memcpy(*result, uberbuf+i, size-i);
      i=size;
    } else {
      if (uberbuf[i] == '\n' || uberbuf[i] == '\r') {
	first++;
      } else {
	first = 0;
      }
    }
  }
  free(uberbuf);

  if (*result == NULL) {
    return(-2);
  } else if (*result[0] == '<') {
    fprintf(stderr, "ERROR: get_http_file() bad file %s\n", file);
    return(-1);
  }
  if(HTTPCACHE) {
    for (i=0; i<2048; i++) {
      if (hcache[i] == NULL) {
	hcache[i] = strdup(*result);
	hcachefiles[i] = strdup(file);
      break;
      }
    }
  }
  return(1);
}

int bqp_get_inverted(char *mach, char *queue, int nodes, int reqtime, int deadline, double needprob, double *result) {
  unsigned int ts;
  int maxc, i, rc;
  char *hbuf, *start, *saveptr;
  char hfile[1024];
  double min, max, prob, quant, lastprob;
  int done=0, loopcount=0;

  if (mach == NULL) {
    printf("ERROR: must supply a machine name using '-m'\n");
    return(-1);
  } else if (queue == NULL) {
    printf("ERROR: must supply a queue name using '-q'\n");
    return(-1);
  } else if (nodes == 0) {
    printf("ERROR: must supply a job node number using '-n'\n");
    return(-1);
  } else if (reqtime == 0) {
    printf("ERROR: must supply a job request time using '-r'\n");
    return(-1);
  }

  sprintf(hfile, "/batchqdata_v2/%s/%s/output.rnprod.cnos", mach, queue);
  rc = get_http_file(HTTPHOST, hfile, &hbuf);
  if (rc < 1) {
    printf("invalid URL %s\n", hfile);
    return(-1);
  }

  maxc = -1;
  start = hbuf;
  start = strtok_r(start, " ", &saveptr);
  while(start != NULL) {
    maxc = atoi(start);
    start = strtok_r(NULL, " ", &saveptr);
  }
  free(hbuf);

  if (maxc == -1) {
    printf("error: cannot retrive cnos\n");
    return(-1);
  }
  
  for (i=0; i<=maxc; i++) {
    sprintf(hfile, "/batchqdata_v2/%s/%s/output.rnprod.%d.range", mach, queue, i);
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc < 1) {
      printf("invalid URL %s\n", hfile);
	return(-1);
    }
    sscanf(hbuf, "%lf %lf", &min, &max);
    free(hbuf);
    
    //    printf("MM: %f %f %f\n", min, max, (double)(nodes * reqtime));
    if ((double)(nodes * reqtime) >= min && (double)(nodes * reqtime) <= max) {
      sprintf(hfile, "/batchqdata_v2/%s/%s/output.rnprod.%d.inverted", mach, queue, i);
      get_http_file(HTTPHOST, hfile, &hbuf);
      start = hbuf;
      start = strtok_r(start, "\n", &saveptr);
      while(start != NULL) {
	sscanf(start, "%u %lf %lf", &ts, &quant, &prob);

	if ((int)quant >= deadline) {
	  *result = lastprob;
	  if (*result < 0.0) {
	    *result = 0.0;
	  }

	  free(hbuf);
	  return(1);
	}
	start = strtok_r(NULL, "\n", &saveptr);
	lastprob = prob;
      }
      free(hbuf);
    }
  }
  if (*result < 0.0) {
    *result = 0.0;
  }
  loopcount++;
  
  if (loopcount > 25) {
    done++;
  }
  *result = 0.0;
  return(-1);
}

int bqp_get_inverted_hmm(bqp *in, int insec, double *result1, double *result2) {
  char hfile[1024];
  char *start, *hbuf, *res, *queue, *saveptr;
  int rc, max, min, i;
  unsigned int ts;
  double meth1, meth2, lastp1, lastp2;
  int seconds, lastsec;
  char *chomp;
  
  *result1 = 0.0;
  *result2 = 0.0;

  //  in->preds = malloc(sizeof(float)*3);

  //  strcpy(in->quantiles[0],".5");
  //  strcpy(in->quantiles[1],".75");
  strcpy(in->quantiles[0],".95");

  //  strcpy(in->confs[0],".1");
  //  strcpy(in->confs[1],".05");
  strcpy(in->confs[0],".05");

  //  in->emps = malloc(sizeof(float)*3);
  in->size = 1;

  res = in->res->resource;
  queue = in->res->queues[in->res->qidx];

  if (in->nodes > 0) {
    snprintf(hfile, 1024, "/batchqdata_v2/%s/ranges", res);
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc <= 0) {
      return(0);
    }
    
    start = hbuf;
    start = strtok_r(start, " ", &saveptr);
    while(start != NULL) {
      chomp = strrchr(start, '\n');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      chomp = strrchr(start, '\r');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      sscanf(start, "%d.%d", &max, &min);
      in->range_min = min;
      in->range_max = max;
      if (in->nodes >= min && in->nodes <= max) {
	break;
      }
      start = strtok_r(NULL, " ", &saveptr);
    }
    free(hbuf);
  } else if (in->nodes == 0) {
    in->range_min = 0;
    in->range_max = 0;
  } else {
    return(0);
  }

  for (i=0; i<in->size; i++) {
    char line[1024];
    int size;

    if (in->nodes == 0) {
      snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.all.%s.inverted", res, queue, res, queue, in->quantiles[i]);
    } else {
      snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.%d.%d.%s.inverted", res, queue, res, queue, in->range_max, in->range_min, in->quantiles[i]);
    }
    
    //    printf("%s\n", hfile);
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc < 0) {
      return(0);
    }   


    {
      lastsec = 0;
      lastp1 = 0.0;
      lastp2 = 0.0;

      start = hbuf;
      start = strtok_r(start, "\n", &saveptr);
      while(start != NULL) {
        chomp = strrchr(start, '\n');
	if (chomp != NULL) {
	  *chomp = '\0';
	}

        chomp = strrchr(start, '\r');
	if (chomp != NULL) {
	  *chomp = '\0';
	}

	sscanf(start, "%u %d %lf %lf", &ts, &seconds, &meth1, &meth2);
	if (seconds > insec) {
	  *result1 = (double)(lastp1 + ( ( (double)(insec-lastsec) / (double)(seconds-lastsec)) * (meth1 - lastp1)));
	  *result2 = (double)(lastp2 + ( ( (double)(insec-lastsec) / (double)(seconds-lastsec)) * (meth2 - lastp2)));
	  free(hbuf);
	  return(1);
	}
	lastp1 = meth1;
	lastp2 = meth2;
	lastsec = seconds;

	start = strtok_r(NULL, "\n", &saveptr);
      }
      free(hbuf);
    }
  }
  *result1 = lastp1;
  *result2 = lastp2;
  return(1);
}

int bqp_get_prediction(char *mach, char *queue, int nodes, int reqtime, double quantile, double *ret) {
  char httpfile[1024], hfile[512];
  char *hbuf, *start, *chomp, *saveptr;
  int maxc, rc, m, q, count, i;
  unsigned int ts;
  double rmin, rmax, preda, predb, predc;
  double rmins[5], rmaxs[5], preds[5], rnprod;
  bqp_res myres;

  sprintf(hfile, "/batchqdata_v2/%s/%s/output.rnprod.cnos", mach, queue);
  rc = get_http_file(HTTPHOST, hfile, &hbuf);
  if (rc < 1) {
    printf("invalid machine/queue combo %s/%s\n", mach, queue);
    return(-1);
  }

  maxc = -1;
  start = hbuf;
  start = strtok_r(start, " ", &saveptr);
  while(start != NULL) {
    maxc = atoi(start);
    start = strtok_r(NULL, " ", &saveptr);
  }
  free(hbuf);

  if (maxc == -1) {
    printf("error: cannot retrive cnos\n");
    return(-1);
  }

  snprintf(httpfile, 1024, "/batchqdata_v2/%s/%s/predcurr.rnprod", mach, queue);
  
  rc = get_http_file(HTTPHOST, httpfile, &hbuf);
  if (rc == -2) {
    printf("mach: %s queue: %s - not enough data\n\n", mach, queue);
  } else if (rc == -1) {
    printf("mach: %s queue: %s - bad/queue machine combination\n\n", mach, queue);
  } else {
    count=0;
    start = hbuf;
    start = strtok_r(start, "\n", &saveptr);
    while(start != NULL) {
      chomp = strrchr(start, '\n');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      
      chomp = strrchr(start, '\r');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      sscanf(start, "%u %lf %lf %lf %lf %lf", &ts, &rmin, &rmax, &preda, &predb, &predc);
      
      rmins[count] = rmin;
      rmaxs[count] = rmax;
      
      if (quantile == 1.0 - 0.05) {
	preds[count] = preda;
      } else if (quantile == 1.0 - 0.25) {
	preds[count] = predb;
      } else if (quantile == 1.0 - 0.5) {
	preds[count] = predc;
      } else {
	printf("invalid quantile specified, try 0.95, 0.75, or 0.5\n");
      }
      
      start = strtok_r(NULL, "\n", &saveptr);
      count++;
    }
    free(hbuf);
  }

  rnprod = nodes * reqtime;
  *ret = (double)preds[count-1];
  for (i=0; i<count; i++) {
    if (rnprod >= rmins[i] && rnprod <= rmaxs[i]) {
      *ret = (double)preds[i];
    }
  }
  return(1);
}


int bqp_get_prediction_hmm(bqp *in) {
  char hfile[1024];
  char *start, *hbuf, *res, *queue, *chomp, *saveptr;
  int rc, max, min, i;

  //  in->preds = malloc(sizeof(float)*3);

  strcpy(in->quantiles[0],".5");
  strcpy(in->quantiles[1],".75");
  strcpy(in->quantiles[2],".95");

  strcpy(in->confs[0],".1");
  strcpy(in->confs[1],".01");
  strcpy(in->confs[2],".05");

  //  in->emps = malloc(sizeof(float)*3);
  in->size = SIZE;

  res = in->res->resource;
  queue = in->res->queues[in->res->qidx];

  if (in->nodes > 0) {
    snprintf(hfile, 1024, "/batchqdata_v2/%s/ranges", res);
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc < 0) {
      return(0);
    }
    
    start = hbuf;
    start = strtok_r(start, " ", &saveptr);
    while(start != NULL) {
      chomp = strrchr(start, '\n');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      chomp = strrchr(start, '\r');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      sscanf(start, "%d.%d", &max, &min);
      in->range_min = min;
      in->range_max = max;
      if (in->nodes >= min && in->nodes <= max) {
	break;
      }
      start = strtok_r(NULL, " ", &saveptr);
    }
    free(hbuf);
  } else if (in->nodes == 0) {
    in->range_min = 0;
    in->range_max = 0;
  } else {
    return(0);
  }

  for (i=0; i<in->size; i++) {
    char line[1024];
    int size;

    if (in->nodes == 0) {
      snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.all.%s.%s.pred.curr", res, queue, res, queue, in->quantiles[i], in->confs[i]);
    } else {
      snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.%d.%d.%s.%s.pred.curr", res, queue, res, queue, in->range_max, in->range_min, in->quantiles[i], in->confs[i]);
    }
    
    //    printf("%s\n", hfile);
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc < 0) {
      return(0);
    }   
    
    //    printf("H: %s\n", hbuf);
    {
      unsigned int ts;
      float pred, acc;
      int vals;
      char *chomp;

      start = hbuf;
      start = strtok_r(start, "\n", &saveptr);
      while(start != NULL) {
        chomp = strrchr(start, '\n');
	if (chomp != NULL) {
	  *chomp = '\0';
	}

        chomp = strrchr(start, '\r');
	if (chomp != NULL) {
	  *chomp = '\0';
	}

	sscanf(start, "%u %f %f %d", &ts, &pred, &acc, &vals);
	start = strtok_r(NULL, "\n", &saveptr);
      }
      free(hbuf);
      //      printf("%u %f %f %d\n", ts, pred, acc, vals);
      in->preds[i] = pred;
      in->accs[i] = acc;
      in->numvals = vals;
      in->ts = ts;
    }
  }
  return(1);
}
int bqp_print_all_predictions(int verbose, char *defmach, char *defqueue, int defnodes, int defreqtime, double quant, double conf) {
  return(bqp_print_all_predictions_internal(verbose, defmach, defqueue, defnodes, defreqtime, quant, conf, NULL, NULL));
}

int bqp_print_all_predictions_internal(int verbose, char *defmach, char *defqueue, int defnodes, int defreqtime, double quant, double conf, winner *retwinners, int *numwinners) {
  
  char httpfile[1024];
  char *hbuf, *start, *chomp, *saveptr;
  double preda, predb, predc, rmin, rmax;
  unsigned int ts;
  double staticnodes[10], staticrtimes[10];
  char staticqueues[128][128][128];
  double table[4][4];
  int i, j, k, last, count, printed, rc, rtimenum, nodenum, lastnode, lastrtime, lastnodestart, lastrtimestart, clustercount;
  double rmins[5], rmaxs[5], preds[5], rnprod;
  char pbuf[256];
  bqp_res *theres, *myres;
  int numres, m, q, currwinner; 
  winner winners[1024];
  int niceminutes;
  char tstr[2];
  char *obuf;

  obuf = (char *)malloc(sizeof(char) * 262144);
  bzero(obuf, sizeof(char)*262144);
  currwinner=0;
  hbuf = NULL;

  //  defnodes = 1;

  if (defnodes <= 0) {
    //    staticnodes = (double *)malloc(sizeof(double)*4);
    staticnodes[0] = 4.0;
    staticnodes[1] = 16.0;
    staticnodes[2] = 64.0;
    staticnodes[3] = 32768.0;
    lastnodestart = 0;
    nodenum = 4;
  } else {
    //    staticnodes = (double *)malloc(sizeof(double)*4);
    staticnodes[0] = (double)defnodes;
    lastnodestart = defnodes - 1;
    nodenum = 1;
  }

  if (defreqtime <= 0) {
    //    staticrtimes = (double *)malloc(sizeof(double)*4);
    staticrtimes[0] = 600.0;
    staticrtimes[1] = 3600.0;
    staticrtimes[2] = 43200.0;
    staticrtimes[3] = 604800.0;
    lastrtimestart = 0;
    rtimenum = 4;
  } else {
    //    staticrtimes = (double *)malloc(sizeof(double)*4);
    staticrtimes[0] = (double)defreqtime;
    lastrtimestart = defreqtime - 1;
    rtimenum = 1;
  }

  rc = bqp_get_resources(&theres, &numres);
  if (rc <= 0) {
    return(-1);
  }
  myres = &theres[0];
  //  printf("NRESSY: %d\n", numres);

  if (defmach != NULL) {
    myres = NULL;
    for (i=0; i<numres; i++) {
      if (!strcmp(theres[i].resource, defmach)) {
	myres = &theres[i];
	myres->qidx = myres->default_qidx;
	numres = 1;
	break;
      }
    }
    if (myres == NULL) {
	sprintf(obuf, "%s%s bad machine specification\n",obuf);
	return(-1);
    }
  }

  if (defqueue == NULL) {
    for (i=0; i<numres; i++) {
      for (j=0; j<myres[i].numqueues; j++) {
	//	printf("%s\n", myres[i].queues[j]);
	strcpy(staticqueues[i][j], myres[i].queues[j]);
      }
    }
  } else {
    for (i=0; i<numres; i++) {
      myres[i].numqueues = 1;
      strcpy(myres[i].queues[0], defqueue);
      strcpy(staticqueues[i][0], defqueue);
    }
  }

  //  printf("NUMRES: %d \n", numres);
  for (m=0; m<numres; m++) {
    //    printf("NUMQUEUES: %d\n", myres[m].numqueues);
    if (!strcmp(myres[m].resource, "fnalcondor")) {
    } else {
      for (q=0; q<myres[m].numqueues; q++) {
	snprintf(httpfile, 1024, "/batchqdata_v2/%s/%s/predcurr.rnprod", myres[m].resource, staticqueues[m][q]);
	
	//      sprintf(httpfile, "/batchqdata_v2/%s/%s/predcurr.rtime", myres[m].resource, staticqueues[m][q]);
	
	rc = get_http_file(HTTPHOST, httpfile, &hbuf);
	if (rc == -2) {
	  if (verbose) {
#ifdef SDSC
#else
	    sprintf(obuf, "%smach: %s queue: %s - not enough data\n\n", obuf,myres[m].resource, staticqueues[m][q]);
#endif
	  }
	} else if (rc == -1) {
	  if (verbose) {
	    sprintf(obuf, "%smach: %s queue: %s - bad/queue machine combination\n\n",obuf,myres[m].resource, staticqueues[m][q]);
	  }
	} else {
	  
	  count=0;
	  start = hbuf;
	  start = strtok_r(start, "\n", &saveptr);
	  while(start != NULL) {
	    chomp = strrchr(start, '\n');
	    if (chomp != NULL) {
	      *chomp = '\0';
	    }
	    
	    chomp = strrchr(start, '\r');
	    if (chomp != NULL) {
	      *chomp = '\0';
	    }
	    sscanf(start, "%u %lf %lf %lf %lf %lf", &ts, &rmin, &rmax, &preda, &predb, &predc);
	  //    printf("%u %f %f %f %f %f\n", ts, rmin, rmax, preda, predb, predc);
	    rmins[count] = rmin;
	    rmaxs[count] = rmax;
	    
	    if (quant == 1.0 - 0.05) {
	      preds[count] = preda;
	    } else if (quant == 1.0 - 0.25) {
	      preds[count] = predb;
	    } else if (quant == 1.0 - 0.5) {
	      preds[count] = predc;
	    } else {
	      preds[count] = preda;
	    }
	    
	    start = strtok_r(NULL, "\n", &saveptr);
	    count++;
	  }
	  free(hbuf);
	  clustercount = count;
	  if (verbose) {
	    rc = sprintf(obuf, "%smach: %s queue: %s - ok\n", obuf,myres[m].resource, staticqueues[m][q]);
	    rc = sprintf(obuf, "%s%8s", obuf,"");
	  }
	  lastrtime = lastrtimestart;
	  for (j=0; j<rtimenum; j++) {
#ifdef SDSC
	    {
	      int minminutes, maxminutes;
	      char mintstr[2], maxtstr[2];
	      strcpy(mintstr, "m");
	      strcpy(maxtstr, "m");
	      minminutes = (lastrtime / 60)+1;
	      if (minminutes >= 60 && minminutes < 1440) {
		minminutes /= 60;
		strcpy(mintstr, "h");
	      } else if (minminutes >= 1440) {
		minminutes = minminutes / 60 / 24;
		strcpy(mintstr, "d");
	      }
	      maxminutes = ((int)staticrtimes[j] / 60);
	      if (maxminutes >= 60 && maxminutes < 1440) {
		maxminutes /= 60;
		strcpy(maxtstr, "h");
	      } else if (maxminutes >= 1440) {
		maxminutes = maxminutes / 60 / 24;
		strcpy(maxtstr, "d");
	      }
	      
	      snprintf(pbuf, 256, "%d%s-%d%s", minminutes, mintstr, maxminutes, maxtstr);
	    }
#else
	    snprintf(pbuf, 256, "%d-%d", lastrtime+1, (int)staticrtimes[j]);
#endif
	    if (verbose) {
	      sprintf(obuf, "%s%15s", obuf,pbuf);
	    }
	    lastrtime = (int)staticrtimes[j];
	  }
	  if (verbose) {
	    sprintf(obuf, "%s\n---------------------------------------------------------------------------\n",obuf);
	  }
	  lastnode = lastnodestart;
	  for(i=0; i<nodenum; i++) {
	    sprintf(pbuf, "%d-%d", lastnode+1, (int)staticnodes[i]); 
	    if (verbose) {
	      sprintf(obuf, "%s%-8s", obuf,pbuf);
	    }
	    lastnode = (int)staticnodes[i];
	    for (j=0; j<rtimenum; j++) {
	      printed = 0;
#ifdef SDSC
	      niceminutes = (int)staticrtimes[j] / 60;
	      strcpy(tstr, "m");
	      if (niceminutes >= 60 && niceminutes <= 1440) {
		niceminutes /= 60;
		strcpy(tstr, "h");
	      } else if (niceminutes > 1440) {
		niceminutes = niceminutes / 60 / 24;
		strcpy(tstr,"d");
	      }
#else
	      niceminutes = (int)staticrtimes[j];
	      strcpy(tstr, "");
#endif
	      for (k=0; k<clustercount; k++) {
		rnprod = staticnodes[i] * staticrtimes[j];
		if ((rnprod >= rmins[k] && rnprod <= rmaxs[k])) {
		  if (verbose) {
		    sprintf(obuf, "%s%15.0f", obuf,preds[k]);
		  }
		  winners[currwinner].num = preds[k];
		  sprintf(winners[currwinner].str, "%8s/%-8s nodes:%-4d walltime:%d%s", myres[m].resource, staticqueues[m][q], (int)staticnodes[i], niceminutes, tstr);
		  currwinner++;
		  printed = 1;
		  k=6;
		}
	      }
	      if (!printed) {
		if (verbose) {
		  sprintf(obuf, "%s%15.0f", obuf,preds[clustercount-1]);
		}
		winners[currwinner].num = preds[clustercount-1];
		
		sprintf(winners[currwinner].str, "%8s/%-8s nodes:%-4d walltime:%d%s", myres[m].resource, staticqueues[m][q], (int)staticnodes[i], niceminutes, tstr);
		currwinner++;
	      }
	    }
	    if (verbose) {
	      sprintf(obuf,"%s\n",obuf);
	    }
	  }
	  if (verbose) {
	    sprintf(obuf,"%s\n",obuf);
	  }
	}
      }
    }
  }
  
  if (retwinners != NULL) {
    memcpy(retwinners, winners, sizeof(winner)*currwinner);
    qsort(retwinners, currwinner, sizeof(winner), sort_by_num);
    *numwinners = currwinner;
  }
  
#ifdef SDSC
  fprintf(FH, "%s",obuf);
#endif
  printf("%s",obuf);
  free(obuf);
  return(1);
}

int bqp_get_all_predictions_hmm(int verbose, char *defmach, char *defqueue, int defnodes, int defreqtime, double quant, double conf) {
  char hfile[1024];
  char *start, *hbuf, *res, *queue, *chomp, *saveptr;
  int rc, max, min, i, j, k;
  bqp_res *theres, *myres;
  bqp *in, thebqp;
  int numres, rangenum;
  char reqranges[4][32] = {"0-600", "601-3600", "3601-43200", "43201-9999999"};
  char noderanges[4][32] = {"1-4", "5-16", "17-64", "65-inf"};
  unsigned int ts;
  float pred, acc;
  int vals, wewon, z;
  int mins[100], rmins[100];
  int maxs[100], rmaxs[100];
  //  char *defqueue;
  //  int defnodes, defreqtime;
  winner winners[10];

  //  defqueue = NULL;
  //  defnodes = 0;
  //  defreqtime = 601;

  for (i=0; i<10; i++) {
    winners[i].num = -1.0;
  }

  bzero(mins, sizeof(int) * 100);
  bzero(maxs, sizeof(int) * 100);
  bzero(rmins, sizeof(int) * 100);
  bzero(rmaxs, sizeof(int) * 100);
  for (i=0; i<4; i++) {
    int rmin, rmax;
    sscanf(reqranges[i], "%d-%d", &rmin, &rmax);
    rmins[i] = rmin;
    rmaxs[i] = rmax;
    //    printf("%d %d\n", rmins[i], rmaxs[i]);
  }

  rc = bqp_get_resources(&theres, &numres);
  myres = NULL;
  for (i=0; i<numres; i++) {
    if (!strcmp(defmach, theres[i].resource)) {
      myres = &theres[i];
      myres->qidx = myres->default_qidx;
      break;
    }
  }
  if (myres == NULL) {
    return(-1);
  }
  in = &thebqp;
  in->res = myres;
  if (quant == 0.95) {
    strcpy(in->quantiles[0],".95");
  } else if (quant == 0.75) {
    strcpy(in->quantiles[0],".75");
  } else if (quant == 0.5) {
    strcpy(in->quantiles[0],".5");
  } else {
    strcpy(in->quantiles[0],".95");
  }

  if (conf == (1 - 0.99)) {
    strcpy(in->confs[0],".01");
  } else if (conf == (1 - 0.95)) {
    strcpy(in->confs[0],".05");
  } else if (conf == (1 - 0.9)) {
    strcpy(in->confs[0],".1");
  } else {
    strcpy(in->confs[0],".05");
  }
  strcpy(in->res->resource, defmach);

  in->size = 1;

  res = in->res->resource;
  if (verbose == 2) {
    in->res->numqueues = 1;
    strncpy(in->res->queues[0], defqueue, 32);
    verbose = 0;
  }
  for (i=0; i<in->res->numqueues; i++) {
    queue = in->res->queues[i];
    //    printf("%s %s\n", res, queue);

    if (verbose == 0) {
      printf("queue: %s\n", queue);
      for (j=0; j<4; j++) {
	printf("%15s", reqranges[j]);
      }
      printf("\n");
    }
    snprintf(hfile, 1024, "/batchqdata_v2/%s/ranges", res);
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc < 0) {
      return(0);
    }
  
    j = 0;
    start = hbuf;
    start = strtok_r(start, " ", &saveptr);
    while(start != NULL) {
      chomp = strrchr(start, '\n');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      chomp = strrchr(start, '\r');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      sscanf(start, "%d.%d", &max, &min);
      //      printf("J: %d %d %d\n", j, min, max);
      mins[j] = min;
      maxs[j] = max;
      j++;
      start = strtok_r(NULL, " ", &saveptr);
    }
    free(hbuf);
    
    rangenum = j;

    for (j=0; j<rangenum; j++) {
      char foo[32];
      
      
      sprintf(foo, "%d-%d", mins[j], maxs[j]);

      if (verbose == 0) {
	printf("%-10s", foo);
      }

      snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.%d.%d.%s.%s.pred.curr", res, queue, res, queue, maxs[j], mins[j], in->quantiles[0], in->confs[0]);
      rc = get_http_file(HTTPHOST, hfile, &hbuf);
      if (rc < 0) {
	//	snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.all.%s.%s.pred.curr", res, queue, res, queue, in->quantiles[0], in->confs[0]);
	//	rc = get_http_file(HTTPHOST, hfile, &hbuf);
	//	printf("%d\n", rc);
	//	if (rc < 0) {
	//	  pred = -1.0;
	//	}
	pred = -1.0;
      }
      if (pred == -1.0) {
	pred = 0.0;
      } else {
	start = hbuf;
	start = strtok_r(start, "\n", &saveptr);
	while(start != NULL) {
	  chomp = strrchr(start, '\n');
	  if (chomp != NULL) {
	    *chomp = '\0';
	  }
	  
	  chomp = strrchr(start, '\r');
	  if (chomp != NULL) {
	    *chomp = '\0';
	  }
	  
	  sscanf(start, "%u %f %f %d", &ts, &pred, &acc, &vals);
	  //	  printf("\n FOO: %f %s\n", pred, foo);
	  start = strtok_r(NULL, "\n", &saveptr);
	}
	free(hbuf);
      }

      // do 0 first
      if (verbose == 0) {
	printf("%5d", (int)pred);
      }
      
      if ((int)pred != 0 && (defqueue == NULL || !strcmp(defqueue, queue)) && (defnodes == 0 || (defnodes >= mins[j] && defnodes <= maxs[j])) && (defreqtime == 0 || (defreqtime >= rmins[0] && defreqtime <= rmaxs[0]))) {
	wewon = 0;
	for (z=9; z>=0; z--) {
	  if (winners[z].num == pred) {
	    break;
	  }
	  if (winners[z].num == -1.0) {
	    winners[z].num = pred;
	    sprintf(winners[z].str, "%s %d-%d %s %f", queue, mins[j], maxs[j], reqranges[0], pred);
	    wewon = 1;
	    qsort(winners, 10, sizeof(winner), sort_by_num_inv);
	    break;
	  }
	}
	if (!wewon) {
	  for (z=9; z>=0; z--) {
	    if (winners[z].num == pred) {
	      break;
	    }
	    if (winners[z].num > pred) {
	      winners[z].num = pred;
	      sprintf(winners[z].str, "%s %d-%d %s %f", queue, mins[j], maxs[j], reqranges[0], pred);
	      qsort(winners, 10, sizeof(winner), sort_by_num_inv);
	      break;
	    }
	  }
	}
      }

      for (k=1; k<4; k++) {
	if (verbose == 0) {
	  printf("%15d", (int)pred);
	}
	
	if (defreqtime && (int)pred != 0 && (defqueue == NULL || !strcmp(defqueue, queue)) && (defnodes == 0 || (defnodes >= mins[j] && defnodes <= maxs[j])) && (defreqtime == 0 || (defreqtime >= rmins[k] && defreqtime <= rmaxs[k]))) {
	  wewon = 0;
	  for (z=9; z>=0; z--) {
	    if (winners[z].num == pred) {
	      break;
	    }
	    if (winners[z].num == -1.0) {
	      winners[z].num = pred;
	      sprintf(winners[z].str, "%s %d-%d %s %f", queue, mins[j], maxs[j], reqranges[k], pred);
	      wewon = 1;
	      qsort(winners, 10, sizeof(winner), sort_by_num_inv);
	      break;
	    }
	  }
	  if (!wewon) {
	    for (z=9; z>=0; z--) {
	      if (winners[z].num == pred) {
		break;
	      }
	      if (winners[z].num > pred) {
		winners[z].num = pred;
		sprintf(winners[z].str, "%s %d-%d %s %f", queue, mins[j], maxs[j], reqranges[k], pred);
		qsort(winners, 10, sizeof(winner), sort_by_num_inv);
		break;
	      }
	    }
	  }
	}
      }

      if (verbose == 0) {
	printf("\n");
      }
    }

    if (verbose == 0) {
      printf("\n");
    }
  }

  if (verbose == 1) {
    printf("queue  nodes  reqtime  prediction\n");
    printf("---------------------------------\n");
    qsort(winners, 10, sizeof(winner), sort_by_num);
    for (z=0; z<10; z++) {
      if (winners[z].num != -1.0) {
	printf("%s\n", winners[z].str);
      }
    }
  }

  return(1);
}

int bqp_get_sdsc_predictions(int verbose, char *defmach, char *defqueue, int defnodes, int defreqtime, double quant, double conf) {
  char hfile[1024];
  char *start, *hbuf, *res, *queue, *chomp, *saveptr;
  int rc, max, min, i, j, k;
  bqp_res *theres, *myres;
  bqp *in, thebqp;
  int numres, rangenum;
  char reqranges[4][32] = {"0m - 10m", "10m - 1h", "1hr-12h", "12h++"};
  unsigned int ts;
  float pred, acc;
  int vals, wewon, z;
  int mins[100], rmins[100];
  int maxs[100], rmaxs[100];
  //  char *defqueue;
  //  int defnodes, defreqtime;
  winner winners[10];

  //  defqueue = NULL;
  //  defnodes = 0;
  //  defreqtime = 601;

  for (i=0; i<10; i++) {
    winners[i].num = -1.0;
  }

  bzero(mins, sizeof(int) * 100);
  bzero(maxs, sizeof(int) * 100);
  bzero(rmins, sizeof(int) * 100);
  bzero(rmaxs, sizeof(int) * 100);
  for (i=0; i<4; i++) {
    int rmin, rmax;
    sscanf(reqranges[i], "%d-%d", &rmin, &rmax);
    rmins[i] = rmin;
    rmaxs[i] = rmax;
    //    printf("%d %d\n", rmins[i], rmaxs[i]);
  }

  rc = bqp_get_resources(&theres, &numres);
  myres = NULL;
  for (i=0; i<numres; i++) {
    if (!strcmp(defmach, theres[i].resource)) {
      myres = &theres[i];
      myres->qidx = myres->default_qidx;
      break;
    }
  }
  if (myres == NULL) {
    return(-1);
  }
  in = &thebqp;
  in->res = myres;
  if (quant == 0.95) {
    strcpy(in->quantiles[0],".95");
  } else if (quant == 0.75) {
    strcpy(in->quantiles[0],".75");
  } else if (quant == 0.5) {
    strcpy(in->quantiles[0],".5");
  } else {
    strcpy(in->quantiles[0],".95");
  }

  if (conf == (1 - 0.99)) {
    strcpy(in->confs[0],".01");
  } else if (conf == (1 - 0.95)) {
    strcpy(in->confs[0],".05");
  } else if (conf == (1 - 0.9)) {
    strcpy(in->confs[0],".1");
  } else {
    strcpy(in->confs[0],".05");
  }
  strcpy(in->res->resource, defmach);

  in->size = 1;

  res = in->res->resource;
  if (verbose == 2) {
    in->res->numqueues = 1;
    strncpy(in->res->queues[0], defqueue, 32);
    verbose = 0;
  }
  for (i=0; i<in->res->numqueues; i++) {
    queue = in->res->queues[i];
    //    printf("%s %s\n", res, queue);

    if (verbose == 0) {
      printf("queue: %s\n", queue);
      for (j=0; j<4; j++) {
	printf("%15s", reqranges[j]);
      }
      printf("\n");
    }
    snprintf(hfile, 1024, "/batchqdata_v2/%s/ranges", res);
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc < 0) {
      return(0);
    }
  
    j = 0;
    start = hbuf;
    start = strtok_r(start, " ", &saveptr);
    while(start != NULL) {
      chomp = strrchr(start, '\n');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      chomp = strrchr(start, '\r');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      sscanf(start, "%d.%d", &max, &min);
      //      printf("J: %d %d %d\n", j, min, max);
      mins[j] = min;
      maxs[j] = max;
      j++;
      start = strtok_r(NULL, " ", &saveptr);
    }
    free(hbuf);
    
    rangenum = j;

    for (j=0; j<rangenum; j++) {
      char foo[32];
      
      
      sprintf(foo, "%d-%d", mins[j], maxs[j]);

      if (verbose == 0) {
	printf("%-10s", foo);
      }

      snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.%d.%d.%s.%s.pred.curr", res, queue, res, queue, maxs[j], mins[j], in->quantiles[0], in->confs[0]);
      rc = get_http_file(HTTPHOST, hfile, &hbuf);
      if (rc < 0) {
	//	snprintf(hfile, 1024, "/batchqdata_v2/%s/%s/%s.%s.all.%s.%s.pred.curr", res, queue, res, queue, in->quantiles[0], in->confs[0]);
	//	rc = get_http_file(HTTPHOST, hfile, &hbuf);
	//	printf("%d\n", rc);
	//	if (rc < 0) {
	//	  pred = -1.0;
	//	}
	pred = -1.0;
      }
      if (pred == -1.0) {
	pred = 0.0;
      } else {
	start = hbuf;
	start = strtok_r(start, "\n", &saveptr);
	while(start != NULL) {
	  chomp = strrchr(start, '\n');
	  if (chomp != NULL) {
	    *chomp = '\0';
	  }
	  
	  chomp = strrchr(start, '\r');
	  if (chomp != NULL) {
	    *chomp = '\0';
	  }
	  
	  sscanf(start, "%u %f %f %d", &ts, &pred, &acc, &vals);
	  //	  printf("\n FOO: %f %s\n", pred, foo);
	  start = strtok_r(NULL, "\n", &saveptr);
	}
	free(hbuf);
      }
      
      // do 0 first
      if (verbose == 0) {
	printf("%5d", (int)pred);
      }


      if (!strcmp(queue, "express")) {
	if (mins[j] > 32) {
	  pred = 0.0;
	}
      }

      if ((int)pred != 0 && (defqueue == NULL || !strcmp(defqueue, queue)) && (defnodes == 0 || (defnodes >= mins[j] && defnodes <= maxs[j])) && (defreqtime == 0 || (defreqtime >= rmins[0] && defreqtime <= rmaxs[0]))) {
	wewon = 0;
	for (z=9; z>=0; z--) {
	  double check, tmp;
	  if (winners[z].num == pred) {
	    break;
	  }
	  
	  if (winners[z].num == -1.0) {
	    int day, hour, min, sec;

	    winners[z].num = pred;
	    if (!strcmp(queue, "express")) {
	      if (maxs[j] > 32) {
		winners[z].num = 99999999999.0;
	      }
	    }
	    day = (int)(pred / 86400);
	    hour = (int)((pred - (day*86400)) / 3600);
	    min = (int)((pred - ((day*86400) + (hour*3600))) / 60);
	    
	    sprintf(winners[z].str, "%-18s %-3d - %-7d %-18s %d:%02d:%02d", queue, mins[j], maxs[j], reqranges[0], day, hour, min);
	    wewon = 1;
	    qsort(winners, 10, sizeof(winner), sort_by_num_inv);
	    break;
	  }
	}
	if (!wewon) {
	  for (z=9; z>=0; z--) {
	    if (winners[z].num == pred) {
	      break;
	    }
	    /*	    if (!strcmp(queue, "express")) {
	      if (maxs[j] > 32) {
		break;
	      }
	    }
	    */
	    if (winners[z].num > pred) {
	      int day, hour, min;
	      winners[z].num = pred;
	      if (!strcmp(queue, "express")) {
		if (maxs[j] > 32) {
		  winners[z].num = 99999999999.0;
		}
	      }

	    day = (int)(pred / 86400);
	    hour = (int)((pred - (day*86400)) / 3600);
	    min = (int)((pred - ((day*86400) + (hour*3600))) / 60);

	      sprintf(winners[z].str, "%-18s %-3d - %-7d %-18s %d:%02d:%02d", queue, mins[j],maxs[j], reqranges[0], day, hour, min);
	      qsort(winners, 10, sizeof(winner), sort_by_num_inv);
	      break;
	    }
	  }
	}
      }

      for (k=1; k<4; k++) {
	if (verbose == 0) {
	  printf("%15d", (int)pred);
	}
	
	if (defreqtime && (int)pred != 0 && (defqueue == NULL || !strcmp(defqueue, queue)) && (defnodes == 0 || (defnodes >= mins[j] && defnodes <= maxs[j])) && (defreqtime == 0 || (defreqtime >= rmins[k] && defreqtime <= rmaxs[k]))) {
	  wewon = 0;
	  for (z=9; z>=0; z--) {
	    if (winners[z].num == pred) {
	      break;
	    }
	    /*	    if (!strcmp(queue, "express")) {
	      if (maxs[j] > 32) {
		break;
	      }
	    }
	    */
	    if (winners[z].num == -1.0) {
	      int day, hour, min;
	      winners[z].num = pred;
		if (!strcmp(queue, "express")) {
		  if (maxs[j] > 32) {
		    winners[z].num = 99999999999.0;
		  }
		}
	    day = (int)(pred / 86400);
	    hour = (int)((pred - (day*86400)) / 3600);
	    min = (int)((pred - ((day*86400) + (hour*3600))) / 60);

	      sprintf(winners[z].str, "%-18s %-3d - %-7d %-18s %d:%02d:%02d", queue, mins[j],maxs[j], reqranges[k], pred);
	      wewon = 1;
	      qsort(winners, 10, sizeof(winner), sort_by_num_inv);
	      break;
	    }
	  }

	  if (!wewon) {
	    for (z=9; z>=0; z--) {
	      if (winners[z].num == pred) {
		break;
	      }
	      if (winners[z].num > pred) {
		int day, hour, min;
		winners[z].num = pred;

		if (!strcmp(queue, "express")) {
		  if (maxs[j] > 32) {
		    winners[z].num = 99999999999.0;
		  }
		}
	    day = (int)(pred / 86400);
	    hour = (int)((pred - (day*86400)) / 3600);
	    min = (int)((pred - ((day*86400) + (hour*3600))) / 60);
	    
	    sprintf(winners[z].str, "%-18s %-3d - %-7d %-18s %d:%02d:%02d", queue, mins[j],maxs[j], reqranges[k], pred);
		qsort(winners, 10, sizeof(winner), sort_by_num_inv);
		break;
	      }
	    }
	  }
	}
      }

      if (verbose == 0) {
	printf("\n");
      }
    }

    if (verbose == 0) {
      printf("\n");
    }
  }

  if (verbose == 1) {
    printf("%-18s %-12s %-18s %-18s\n", "queue", "processors", "wall_clock_time", "latest_queue_wait_time");
    printf("--------------------------------------------------------------------------------\n");
    qsort(winners, 10, sizeof(winner), sort_by_num);
    for (z=0; z<10; z++) {
      if (winners[z].num != -1.0) {
	printf("%s\n", winners[z].str);
      }
    }
  }

  return(1);
}

int bqp_get_resources(bqp_res **in, int *numres) {
  int rc, count, i, qcount, fd, normal;
  char hfile[1024];
  char *hbuf, *hbufa, *start, *chomp, *saveptr;
  bqp_res *currres;

  normal = 0;
  if (DOCACHE) {
    fd = open("rescache", O_RDONLY);
    if (fd < 0) {
      normal = 1;
    } else {
      hbuf = (char *)malloc(32768);
      rc = read(fd, hbuf, 32768);
      *in = (bqp_res *)malloc(rc);
      memcpy(*in, hbuf, rc);
      close(fd);
      *numres = rc / sizeof(bqp_res);
    }
  } else {
    normal = 1;
  }
  
  if (normal) {
    snprintf(hfile, 1024, "/batchqdata_v2/machines");
    rc = get_http_file(HTTPHOST, hfile, &hbuf);
    if (rc <= 0) {
      return(0);
    }
    
    hbufa = strdup(hbuf);
    
    count=0;
    start = hbuf;
    start = strtok_r(start, " ", &saveptr);
    while(start != NULL) {
      count++;
      start = strtok_r(NULL, " ", &saveptr);
    }
    free(hbuf);
    
    *in = (bqp_res *)malloc(sizeof(bqp_res)*count);
    if (*in == NULL) {
      printf("OUT OF MEMORY\n");
      exit(-1);
    }
    *numres = count;
    count=0;
    start = hbufa;
    start = strtok_r(start, " ", &saveptr);
    while(start != NULL) {
      chomp = strrchr(start, '\n');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      chomp = strrchr(start, '\r');
      if (chomp != NULL) {
	*chomp = '\0';
      }
      currres = *in + count;
      strcpy(currres->resource, start);
      count++;
      start = strtok_r(NULL, " ", &saveptr);
    }
    free(hbufa);
    
    
    for (i=0; i<count; i++) {
      currres = *in + i;
      snprintf(hfile, 1024, "/batchqdata_v2/%s/queues", currres->resource);
      rc = get_http_file(HTTPHOST, hfile, &hbuf);
      if (rc < 0) {
	return(0);
      }
      
      snprintf(hfile, 1024, "/batchqdata_v2/%s/defaultq", currres->resource);
      rc = get_http_file(HTTPHOST, hfile, &hbufa);
      if (rc < 0) {
	return(0);
      }
      sscanf(hbufa, "%s", hbufa);
      
      qcount=0;
      start = hbuf;
      start = strtok_r(start, "\n", &saveptr);
      while(start != NULL) {
	chomp = strrchr(start, '\n');
	if (chomp != NULL) {
	  *chomp = '\0';
	}
	chomp = strrchr(start, '\r');
	if (chomp != NULL) {
	  *chomp = '\0';
	}
	
	strcpy(currres->queues[qcount], start);
	if (!strcmp(start, hbufa)) {
	  currres->default_qidx = qcount;
	}
	
	qcount++;
	start = strtok_r(NULL, "\n", &saveptr);
      }
      currres->numqueues = qcount;
      free(hbuf);
      free(hbufa);    
    }
    
    if (DOCACHE) {
      fd = open("rescache", O_WRONLY | O_TRUNC | O_CREAT, 0644);
      write(fd, *in, sizeof(bqp_res)*count);
      close(fd);
    }
    
  }
  //  printf("NRES: %d\n", *numres);
  return(1);
}
