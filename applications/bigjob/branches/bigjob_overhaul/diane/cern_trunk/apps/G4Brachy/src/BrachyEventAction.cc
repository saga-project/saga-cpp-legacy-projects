//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// Code developed by:
//  S.Guatelli
//
//    *******************************
//    *                             *
//    *    BrachyEventAction.cc     *
//    *                             *
//    *******************************
//
// $Id: BrachyEventAction.cc,v 1.1 2008/10/03 16:39:44 moscicki Exp $
// GEANT4 tag $Name:  $
//

#include "BrachyEventAction.hh"
#include "BrachyPhantomHit.hh"
#include "BrachyPhantomSD.hh"
#include "BrachyDetectorConstruction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"

#include <iomanip>

#ifdef G4ANALYSIS_USE
#include"BrachyAnalysisManager.hh"
#endif

#define G4EVENTTIMER_USE 1

// Retrieve information about the energy deposit in the phantom ...

BrachyEventAction::BrachyEventAction() :
  drawFlag("all" )
{ 
  eventUserTimeAcc = 0.0;
  eventNmb = 0; 
  taskNmb = 1;
  // cpu_time_used_acc = 0.0;
}

BrachyEventAction::~BrachyEventAction()
{

}

void BrachyEventAction::BeginOfEventAction(const G4Event*)
{

#ifdef G4EVENTTIMER_USE
   if(eventNmb == 20001) {
     eventNmb=0;
     taskNmb=taskNmb+1;
   }
   eventNmb++;

   if(eventNmb==1) eventTimer.Start();

#endif
}

void BrachyEventAction::EndOfEventAction(const G4Event* evt)
{  
  // extract the trajectories and draw them ...

  if (G4VVisManager::GetConcreteInstance())
    {
      G4TrajectoryContainer * trajectoryContainer = evt->GetTrajectoryContainer();
      G4int n_trajectories = 0;
      if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

      for (G4int i=0; i<n_trajectories; i++) 
        {
          G4Trajectory* trj = (G4Trajectory*)
                                ((*(evt->GetTrajectoryContainer()))[i]);
	  if(drawFlag == "all") trj->DrawTrajectory(50);
	  else if((drawFlag == "charged")&&(trj->GetCharge() != 0.))
	    trj->DrawTrajectory(50);
	  else if ((drawFlag == "neutral")&&(trj->GetCharge() == 0.))
	    trj->DrawTrajectory(50);	     	     
	}
    }

#ifdef G4EVENTTIMER_USE
  //  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  //  cpu_time_used = ((double) (end - start));
  //  cpu_time_used_acc += cpu_time_used;
  //  std::cout << cpu_time_used << "  " << cpu_time_used_acc << std::endl;
 
  if (eventNmb==20000) {
     eventTimer.Stop();
     eventUserTimeAcc += (eventTimer.GetSystemElapsed() + eventTimer.GetUserElapsed());
     std::cout << "Event Timer ("<< eventNmb << "):"  << std::endl;
     std::cout << " Real:   " << eventTimer.GetRealElapsed()   << std::endl;
     std::cout << " System: " << eventTimer.GetSystemElapsed() << std::endl;
     std::cout << " User:   " << eventTimer.GetUserElapsed()   << std::endl;
     std::cout << " User:   " << eventTimer.GetUserElapsed() + eventTimer.GetSystemElapsed()  << std::endl;
     std::cout << "CPU Time (EventNmb=" << eventNmb << "):  "  
               << std::setprecision(8) << eventUserTimeAcc/taskNmb 
               << " sec" << std::endl;
  }

#endif
}
