
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// UserRunAction.cc
// 
// Defining actions performed at the beginning and/or the end of each run
//
/////////////////////////////////////////////////////////////////////////


#include "UserRunAction.hh"
#include "AidaObjectManager.hh"
#include "G4Run.hh"


void UserRunAction::BeginOfRunAction(const G4Run* run) {

  // The run ID is printed at the beginning of each run
  G4cout << "INFORMATION: Run No " << run -> GetRunID() 
         << " starts." << G4endl;
  G4cout << "file =" << fileName << G4endl;

  AidaObjectManager::Instance(fileName); 
}

void UserRunAction::EndOfRunAction(const G4Run* run) {

  // The run ID and the number of processed events are printed at the end of 
  // each run
  G4cout << "INFORMATION: Run No " << run -> GetRunID() 
         << " ends (Number of events = " 
         << run -> GetNumberOfEvent() << ")." << G4endl;

  AidaObjectManager::Destroy();
}
