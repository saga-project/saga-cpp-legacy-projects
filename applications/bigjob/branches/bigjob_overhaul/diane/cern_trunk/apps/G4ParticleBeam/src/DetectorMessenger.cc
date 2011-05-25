
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"


DetectorMessenger::DetectorMessenger(DetectorConstruction* detConstr) :
    detectorConstruction(detConstr) {

  detDirectory = new G4UIdirectory("/detector/");
  detDirectory -> SetGuidance("Detector related commands");

  detMaxStepSizeCmd = 
                  new G4UIcmdWithADoubleAndUnit("/detector/maxStepSize",this);
  detMaxStepSizeCmd -> SetGuidance("Maximum step size in detector");
  detMaxStepSizeCmd -> SetParameterName("maxstepsize", false);
  detMaxStepSizeCmd -> SetRange("maxstepsize>0.");
  detMaxStepSizeCmd -> SetUnitCategory("Length");
  detMaxStepSizeCmd -> AvailableForStates(G4State_Idle);

  detColourCmd = new G4UIcmdWithAString("/detector/colour",this);
  detColourCmd -> SetGuidance("Detector colour");
  detColourCmd -> SetParameterName("colour", false);
  detColourCmd -> AvailableForStates(G4State_Idle);
}


DetectorMessenger::~DetectorMessenger() {
 
  delete detColourCmd;
  delete detMaxStepSizeCmd;
  delete detDirectory;
}


void DetectorMessenger::SetNewValue(G4UIcommand* cmd, G4String val) {

  if(cmd == detMaxStepSizeCmd) {
     detectorConstruction -> SetBoxMaxStepSize(
                               detMaxStepSizeCmd -> GetNewDoubleValue(val));
  }
  if(cmd == detColourCmd) {
     detectorConstruction -> SetBoxColour(val);
  } 

}
