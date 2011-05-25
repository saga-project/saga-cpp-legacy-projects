
#include "ParticleBeamSimulation.hh"
#include "Randomize.hh"  
#include "G4RunManager.hh" 
#include "G4UImanager.hh"
#include "G4UIsession.hh"
#include "G4UIterminal.hh"
#include "G4UImessenger.hh"
#include "G4UItcsh.hh"
#include "globals.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGenerator.hh"
#include "UserRunAction.hh"
#include "UserEventAction.hh"


ParticleBeamSimulation::ParticleBeamSimulation(G4int sd) {
 
  outputFileName = "simoutput.xml";
  runManager = 0;

  seed = sd;
}


ParticleBeamSimulation::~ParticleBeamSimulation() {

}

void ParticleBeamSimulation::setSeed(G4int sd) {

  G4cout << "INFORMATION: Setting random seed = " << sd << G4endl;

  seed = sd;
  CLHEP::HepRandom ::setTheSeed(seed);
}


G4bool ParticleBeamSimulation::initialize(int ,char** ) {
 
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

  G4cout << "INFORMATION: Using seed = " 
         << CLHEP::HepRandom ::getTheSeed()  << G4endl;
 
  G4cout << "INFORMATION: Initializing Geant4 application" << G4endl;
 
  G4RunManager *runManager = G4RunManager::GetRunManager();

  if(runManager) {
     G4cout << "INFORMATION: Instance of G4RunManager exits. No creation required" << std::endl;
  }
  else {
     G4cout << "INFORMATION: Creating new instance of G4RunManager" << std::endl;
     runManager = new G4RunManager;
  }

  G4String detectorRegionName = "Detector"; 
  DetectorConstruction* detector = new DetectorConstruction(detectorRegionName);
  runManager -> SetUserInitialization(detector);  

  PhysicsList* physics = new PhysicsList(detectorRegionName);
  runManager -> SetUserInitialization(physics);

  PrimaryGenerator* source = new PrimaryGenerator(); 
  runManager -> SetUserAction(source);
     
  UserRunAction* runAction = new UserRunAction(outputFileName); 
  runManager -> SetUserAction(runAction);  

  UserEventAction* eventAction = new UserEventAction();
  runManager -> SetUserAction(eventAction);

  runManager -> Initialize();
  return true;
}


void ParticleBeamSimulation::executeMacro(std::string macroFileName) {

  G4UImanager* UI = G4UImanager::GetUIpointer();  

  G4String fileName = macroFileName;
  G4cout << "INFORMATION: Using macro file " << fileName 
         << " for job execution."
         << G4endl;

  G4String command = "/control/execute ";

  if(!UI)
    G4cout << "ERROR: UI pointer does not exist" << G4endl;
  else
    UI -> ApplyCommand(command+fileName);    
}


std::string  ParticleBeamSimulation::getOutputFilename() {

  return outputFileName;
}


void ParticleBeamSimulation::finish() {

  delete runManager;
}

//// This is all the application needs to run in parallel mode through DIANE
extern "C" 
DIANE::IG4Simulation* createG4Simulation(int seed) 
{return new ParticleBeamSimulation(seed); }
///////////
