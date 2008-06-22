
#ifndef INCLUDE_BQP_H
#define INCLUDE_BQP_H

CCTK_INT BQP_GetResources(CCTK_INT * NumRes, char * ResNames, char *Seperator);
CCTK_INT BQP_GetQueues(char * ResNames, char *Queues, char *Seperator);
CCTK_INT BQP_GetBest(CCTK_INT NodeCount, CCTK_INT RequiredTime, CCTK_INT Deadline, char *ResName, char *Queue, CCTK_REAL* Prediction);
CCTK_INT BQP_GetBestFromList(char *MachineNameList, char *Seperator, CCTK_INT NodeCount, CCTK_INT RequiredTime, CCTK_INT Deadline, char *ResName, char *Queue, CCTK_REAL* Prediction);
CCTK_INT BQP_GetTimeInQueue(char *MachineNameList, char *Seperator, CCTK_INT NodeCount, CCTK_INT RequiredTime, char *ResName, char *Queue, CCTK_INT* TimeInQueue);

#endif

