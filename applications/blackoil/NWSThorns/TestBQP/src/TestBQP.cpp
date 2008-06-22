#include <iostream>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include <saga.hpp>

#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"

extern "C"
{
  #include "bqp_data.h"
}
extern "C"
{
  #include "BQP.h"
  #include "cctk.h"
  #include "cctk_Arguments.h"
  #include "cctk_Parameters.h"
  CCTK_INT BQP_GetResources(CCTK_INT * NumRes, char * ResNames, char *Seperator);
  CCTK_INT BQP_GetQueues(char * ResNames, char *Queues, char *Seperator);
  CCTK_INT BQP_GetBest(CCTK_INT NodeCount, CCTK_INT RequiredTime, CCTK_INT Deadline, char *ResName, char *Queue, CCTK_REAL* Prediction);
  CCTK_INT BQP_GetBestFromList(char *MachineNameList, char *Seperator, CCTK_INT NodeCount, CCTK_INT RequiredTime, CCTK_INT Deadline, char *ResName, char *Queue, CCTK_REAL* Prediction);
  CCTK_INT BQP_GetTimeInQueue(char *MachineNameList, char *Seperator, CCTK_INT NodeCount, CCTK_INT RequiredTime, char *ResName, char *Queue, CCTK_INT* TimeInQueue);

}

using namespace std;
using namespace saga;
using namespace saga::advert;

int  TestBQP_SetupAdvertStructure();
int  TestBQP_AddMachineList(char * MachineList);
int  AddTimeToComplete(std::string ResName, int NodeCount, int TimeInQueue);

inline std::string itoa (int num) { std::stringstream converter; converter << num; return converter.str (); }
 // Some functions that yye00 needs for string manipulation and such
  inline void Tokenize (std::string & str,
                        std::vector < std::string > &tokens,
                        const std::string & delimiters = ",")
  {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of (delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of (delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back (str.substr (lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of (delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of (delimiters, lastPos);
    }
  }



extern "C" void TestBQP(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  int i,j,NumMachines, ierror;
  CCTK_REAL Prediction;
  char MachineNames[1024], Queues[1024], Seperator[]={","};
  char *tempMachine = NULL;
  char BestMachine[128], BestQueue[128];
  char MachineList[256]="datastar,bigben,lonestar,ada,abe,eldorado,queenbee";
 
  char TimeMachineName[128];
  char TimeQueue[128];
  CCTK_INT TimeInQueue;
/*
  ierror=TestBQP_SetupAdvertStructure();
  ierror = TestBQP_AddMachineList(MachineList); 
  memset(MachineNames,'\0',1024);
  ierror = BQP_GetResources(&NumMachines, MachineNames, Seperator);
  printf("\n value of Machines is:%s \n", MachineNames);
  

  ierror = BQP_GetBest(2, 25000, 43200, BestMachine, BestQueue, &Prediction);
  printf("\n the best machine is:%s, the best queue is:%s with value:%f\n",BestMachine, BestQueue, Prediction);
  memset(BestMachine,'\0',128);
  memset(BestQueue,'\0',128);
    
  ierror =BQP_GetBestFromList(MachineList, Seperator, 2, 2500, 43200, BestMachine, BestQueue, &Prediction);
  printf("\n Out of list, the best machine is:%s, the best queue is:%s with value:%f\n",BestMachine, BestQueue, Prediction);

  ierror = BQP_GetTimeInQueue(MachineList, Seperator, 1, 2500,TimeMachineName, TimeQueue, &TimeInQueue); 
  printf("\n the time in queue for a 1 node job lasting 2500 is:%d, on resource: %s, queue: %s",TimeInQueue,TimeMachineName,TimeQueue);
*/

  std::string strMachineList(MachineList);
  std::vector<std::string> ResNames;
  std::string strIt;
  Tokenize(strMachineList, ResNames,",");
  typedef std::vector<std::string>::const_iterator iterator;
  iterator end=ResNames.end();
  char buffer[128];

  for(iterator it2=ResNames.begin();it2!=ResNames.end();it2++)
    std::cout<<"Resnames is:"<<*it2<<endl;

  for(iterator it=ResNames.begin(); it!=end; ++it)
  {
    for(i=1;i<=16;i*=2)
    {
      for(j=7200;j<86400;j+=3600)
      {
        memset(buffer,'\0',128);
        strcpy(buffer, (*it).c_str());
        ierror = BQP_GetTimeInQueue(buffer, Seperator, i, j,TimeMachineName, TimeQueue, &TimeInQueue);
        ierror = AddTimeToComplete((*it), i,TimeInQueue);
        printf("\n the time in queue for an: %d node job lasting:%d is:%d, on resource: %s, queue: %s",i,j,TimeInQueue,TimeMachineName,TimeQueue);
      }
    }
  }



}

int  TestBQP_SetupAdvertStructure()
{
  CCTK_INFO("setting up the advert directory structure");
  int mode = advert::ReadWrite;
  std::string advertKey("advert://fortytwo.cct.lsu.edu");
  advertKey += "//yye01";  // add a session id
  try {
    advert::directory(advertKey, mode |advert::Create);
  }
  catch(saga::exception const &e)  {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }

  CCTK_INFO("adding BQP directory");
  try {
   advertKey+= "/BQP/";
   advert::directory(advertKey, mode |advert::Create);
  }
  catch(saga::exception const &e)  {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }

  return 0;

}

int TestBQP_AddMachineList(char *machinelist)
{

  std::string strMachineList(machinelist);

  CCTK_INFO("Adding the machinelist to the advert service");
  int mode = advert::ReadWrite;
  std::string advertKey("advert://fortytwo.cct.lsu.edu//yye01/MachineList");
  try {
    advert::entry advMachineList (advertKey, mode |advert::Create);
    advMachineList.store_string(strMachineList);
  }
  catch(saga::exception const &e)  {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }
  return 0;
}


int  AddTimeToComplete(std::string ResName, int NodeCount, int TimeInQueue)
{
  std::string advertKey("advert://fortytwo.cct.lsu.edu//yye01/BQP/");
  advertKey+=ResName+"_";

  std::string strNodeCount=itoa(NodeCount);
  std::string strTimeInQueue=itoa(TimeInQueue);

  advertKey+= strNodeCount +"_Nodes";
  int mode=advert::ReadWrite;
  try {
    advert::entry advEntry(advertKey, mode |advert::Create);
    advEntry.store_string(strTimeInQueue);
  }
  catch(saga::exception const &e)  {
    std::cout << "FAILED (" << e.get_error() << ")" << std::endl;
    throw;
  }


  return 0;
}

