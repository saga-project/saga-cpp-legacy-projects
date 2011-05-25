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
//
// $Id: Brachy.cc
// GEANT4 tag $Name:  $
//
// --------------------------------------------------------------
//                 GEANT 4 - Brachytherapy example
// --------------------------------------------------------------
//
// Code developed by:
// S. Agostinelli, F. Foppiano, S. Garelli , M. Tropeano, S.Guatelli

//
//    *******************************
//    *                             *
//    *    Brachy.cc                *
//    *                             *
//    *******************************
//
// Brachytherapy simulates the energy deposition in a cubic (30*cm)
//
// brachytherapy source.
//
// Simplified gamma generation is used.
// Source axis is oriented along Z axis. The source is in the centre
//of the box.

//default source Ir-192

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "BrachyFactoryIr.hh"
#include "BrachySimulation.hh"
#include "BrachyEventAction.hh"
#include "BrachyDetectorConstruction.hh"
#include "BrachyPhysicsList.hh"
#include "BrachyPhantomSD.hh"
#include "BrachyPrimaryGeneratorActionIr.hh"
#include "G4SDManager.hh"
#include "BrachyRunAction.hh"
#include "Randomize.hh"  
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4UImessenger.hh"

BrachySimulation::BrachySimulation(G4int sd)
{ 
  pRunManager = 0;
  seed = sd;
}

BrachySimulation::~BrachySimulation()
{;}

void BrachySimulation::setSeed(G4int sd)
{

  G4cout << "INFORMATION: Setting random seed = " << sd << G4endl;

  seed = sd;
  CLHEP::HepRandom ::setTheSeed(seed);
}

G4bool BrachySimulation::initialize(int ,char** )
{ 
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

  G4cout << "INFORMATION: Using seed = " 
         << CLHEP::HepRandom ::getTheSeed()  << G4endl;
 
  G4cout << "G4 initializing" << G4endl;
 
  G4RunManager *pRunManager = G4RunManager::GetRunManager();

  if(pRunManager) {
     std::cout << "Instance of G4RunManager exits. No creation required" << std::endl;
  }
  else {
     std::cout << "Creating new instance of G4RunManager" << std::endl;
     pRunManager = new G4RunManager;
  }
  //G4RunManager* pRunManager = new G4RunManager;

  G4String sensitiveDetectorName = "Phantom";

  BrachyDetectorConstruction  *pDetectorConstruction = new  BrachyDetectorConstruction(sensitiveDetectorName);

  pRunManager->SetUserInitialization(pDetectorConstruction);
  pRunManager->SetUserInitialization(new BrachyPhysicsList);

  // output environment variables:
#ifdef G4ANALYSIS_USE
  G4cout << G4endl << G4endl << G4endl 
	 << " User Environment " << G4endl
	 << " Using AIDA 3.2.1 analysis, PI 1_3_3 " << G4endl;
# else
  G4cout << G4endl << G4endl << G4endl 
	 << " User Environment " << G4endl
	 << " G4ANALYSIS_USE environment variable not set, NO ANALYSIS " 
	 << G4endl;
#endif
 
  BrachyEventAction *pEventAction = new BrachyEventAction();
  pRunManager->SetUserAction(pEventAction );

  BrachyRunAction *pRunAction = new BrachyRunAction(sensitiveDetectorName);
  pRunManager->SetUserAction(pRunAction);

  //Initialize G4 kernel
  pRunManager->Initialize();
  return true;
}
void BrachySimulation::executeMacro(std::string macroFileName)
{
  G4UImanager* UI = G4UImanager::GetUIpointer();  

  // Batch mode

  std::string fileName = macroFileName;
  G4cout << fileName << " <---------- in batch -----------------" << G4endl;
  G4cout << macroFileName << " executed"<<G4endl;

  std::string command = "/control/execute ";

  if(!UI)
    G4cout << "FATAL ERROR: UI pointer does not exist" << G4endl;
  else
    UI -> ApplyCommand(command+fileName);    
}
std::string  BrachySimulation::getOutputFilename()
{
  return "brachytherapy.xml";
}

void BrachySimulation::finish()
{
  delete pRunManager;
}

//// This is all the application needs to run in parallel mode through DIANE
extern "C" 
DIANE::IG4Simulation* createG4Simulation(int seed) 
{return new BrachySimulation(seed); }
///////////
