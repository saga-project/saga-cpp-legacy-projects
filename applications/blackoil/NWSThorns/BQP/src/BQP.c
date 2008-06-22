#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"
#include "bqp_data.h"

#include "BQP.h"

#define  DEBUG 1

CCTK_INT BQP_GetResources(CCTK_INT * NumRes, char * ResNames, char * Separator)
{
  int rc, i, numres;
  /* bqp_res is a struct (see bqp.h) which defines a 'resource' for the api. */
  bqp_res *theres;
  /* the first thing we may want to do is get a list of supported machines and queues.  after bqp_get_resources completes, 'theres' will be an array of bqp_res structs */
  rc = bqp_get_resources(&theres, &numres);
  if (rc <= 0) {
    CCTK_VWarn(1,__LINE__,__FILE__,CCTK_THORNSTRING, "ERROR: bqp_get_resources %d", rc);
    return rc;
  }
  (*NumRes) = numres;
  for (i=0; i<numres; i++)
  {
    strcat(ResNames, theres[i].resource);
    strcat(ResNames, Separator);
  }
  return 0;
}

/* This function should work, testing it proved a bit of a problem */
CCTK_INT BQP_GetQueues(char * ResNames, char *Queues, char *Seperator)
{
  int rc, i, j, numres;
  /* bqp_res is a struct (see bqp.h) which defines a 'resource' for the api. */
  bqp_res *theres;
  /* the first thing we may want to do is get a list of supported machines and queues.  after bqp_get_resources completes, 'theres' will be an array of bqp_res structs */
  rc = bqp_get_resources(&theres, &numres);
  if (rc <= 0) {
    CCTK_VWarn(1,__LINE__,__FILE__,CCTK_THORNSTRING, "ERROR: bqp_get_resources %d\n", rc);
    return -1;
  }
  for (i=0; i<numres; i++)
  {
    if(strcmp(theres[i].resource,ResNames)==0)
    {
      for (j=0; j<theres[i].numqueues; j++)
      {
        strcat(Queues, theres[i].queues[j]);
        strcat(Queues, Seperator);
      }
    }
  }
  return 0;
}

CCTK_INT BQP_GetBest(CCTK_INT NodeCount, CCTK_INT RequiredTime, CCTK_INT Deadline, char *ResName, char *Queue, CCTK_REAL* Prediction)
{
  int rc, i, j, numres;
  int BestMachine, BestQueue;
  CCTK_REAL prediction, BestPrediction=0;

  /* bqp_res is a struct (see bqp.h) which defines a 'resource' for the api. */
  bqp_res *theres;
  /* the first thing we may want to do is get a list of supported machines and queues.  after bqp_get_resources completes, 'theres' will be an array of bqp_res structs */
  rc = bqp_get_resources(&theres, &numres);
  if (rc <= 0) {
    CCTK_VWarn(1,__LINE__,__FILE__,CCTK_THORNSTRING, "ERROR: bqp_get_resources %d\n", rc);
    return -1;
  }
  /* for all machines, for all queues, find the highest probability combo */
  for (i=0; i<numres; i++)
  {
        for (j=0; j<theres[i].numqueues; j++)
    {
      #ifdef DEBUG
      printf("\n Resource:%s, Queue:%s",theres[i].resource, theres[i].queues[j]);
      #endif
      rc = bqp_get_inverted(theres[i].resource, theres[i].queues[j], NodeCount, RequiredTime, Deadline, 0, &prediction);
      #ifdef DEBUG
      printf("\t value of prediction is:%f\n",prediction);
      #endif
      if (prediction > BestPrediction)
      {
        BestMachine = i; BestQueue = j; BestPrediction = prediction;
      }
    }
  }
  strcpy(ResName, theres[BestMachine].resource);
  strcpy(Queue,  theres[BestMachine].queues[BestQueue]);
  (*Prediction) = BestPrediction;
  
  return 0;
}

CCTK_INT BQP_GetBestFromList(char *MachineNameList, char *Seperator, CCTK_INT NodeCount, CCTK_INT RequiredTime, CCTK_INT Deadline, char *ResName, char *Queue, CCTK_REAL* Prediction)
{
  int rc, i, j, numres;
  int BestMachine, BestQueue;
  CCTK_REAL prediction, BestPrediction=0;
  char *tempMachine = NULL;
  
  /* bqp_res is a struct (see bqp.h) which defines a 'resource' for the api. */
  bqp_res *theres;
  /* the first thing we may want to do is get a list of supported machines and queues.  after bqp_get_resources completes, 'theres' will be an array of bqp_res structs */
  rc = bqp_get_resources(&theres, &numres);
  if (rc <= 0)
  {
    CCTK_VWarn(1,__LINE__,__FILE__,CCTK_THORNSTRING, "ERROR: bqp_get_resources %d\n", rc);
    return -1;
  }
  
  tempMachine = strtok(MachineNameList, Seperator);
  while(tempMachine!=NULL)
  {
    for (i=0; i<numres; i++)
    {
      if(strcmp(tempMachine,theres[i].resource)==0)
      {
        for (j=0; j<theres[i].numqueues; j++)
        {
          #ifdef DEBUG
          printf("\n Resource:%s, Queue:%s",theres[i].resource, theres[i].queues[j]);
          #endif
          rc = bqp_get_inverted(theres[i].resource, theres[i].queues[j], NodeCount, RequiredTime, Deadline, 0, &prediction);
          #ifdef DEBUG
          printf("\t value of prediction is:%f\n",prediction);
          #endif
          if (prediction > BestPrediction)
          {
            BestMachine = i; BestQueue = j; BestPrediction = prediction;
          }
        }
      }
    }
    tempMachine = strtok(NULL, Seperator);
  }

  strcpy(ResName, theres[BestMachine].resource);
  strcpy(Queue,  theres[BestMachine].queues[BestQueue]);
  (*Prediction) = BestPrediction;
  
  return 0;
}

CCTK_INT BQP_GetTimeInQueue(char *MachineNameList, char *Seperator, CCTK_INT NodeCount, CCTK_INT RequiredTime, char *ResName,  char *Queue, CCTK_INT *TimeInQueue)
{
  int rc, i, j, numres;
  int BestMachine, BestQueue, BestTimeInQueue=1024;
  CCTK_REAL prediction, BestPrediction=0;
  CCTK_INT  Deadline = 0;
  char *tempMachine = NULL;

  /* bqp_res is a struct (see bqp.h) which defines a 'resource' for the api. */
  bqp_res *theres;
  /* the first thing we may want to do is get a list of supported machines and queues.  after bqp_get_resources completes, 'theres' will be an array of bqp_res structs */
  rc = bqp_get_resources(&theres, &numres);
  if (rc <= 0)
  {
    CCTK_VWarn(1,__LINE__,__FILE__,CCTK_THORNSTRING, "ERROR: bqp_get_resources %d\n", rc);
    return -1;
  }

  tempMachine = strtok(MachineNameList, Seperator);
  while(tempMachine!=NULL)
  {
    for (i=0; i<numres; i++)
    {
      if(strcmp(tempMachine,theres[i].resource)==0)
      {
        for (j=0; j<theres[i].numqueues; j++)
        {
          #ifdef DEBUG
          printf("\n Resource:%s, Queue:%s",theres[i].resource, theres[i].queues[j]);
          #endif
          // search up to 1 week with 0.5 hour increments
          for(Deadline=0;Deadline<432000;Deadline+=1800)
          {
            rc = bqp_get_inverted(theres[i].resource, theres[i].queues[j], NodeCount, RequiredTime, Deadline, 0, &prediction);
            if(prediction>=0.8)
              break;
          }
          #ifdef DEBUG
          printf("\t value of prediction is:%f\n",prediction);
          #endif
          if (Deadline < BestTimeInQueue)
          {
            BestMachine = i; BestQueue = j; BestPrediction = prediction; (*TimeInQueue)=Deadline;
          }
        }
      }
    }
    tempMachine = strtok(NULL, Seperator);
  }

  strcpy(ResName, theres[BestMachine].resource);
  strcpy(Queue,  theres[BestMachine].queues[BestQueue]);

  return 0;
}


