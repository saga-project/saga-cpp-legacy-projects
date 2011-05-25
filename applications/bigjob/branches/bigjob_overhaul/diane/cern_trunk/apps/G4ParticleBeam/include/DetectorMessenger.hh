#ifndef DETECTORMESSENGER_HH
#define DETECTORMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;


class DetectorMessenger : public G4UImessenger {

 public:
   DetectorMessenger(DetectorConstruction*);
   ~DetectorMessenger();

   void SetNewValue(G4UIcommand*, G4String);

 private:
   DetectorConstruction* detectorConstruction;

   G4UIdirectory* detDirectory;
   G4UIcmdWithADoubleAndUnit* detMaxStepSizeCmd;
   G4UIcmdWithAString* detColourCmd;
};

#endif // DETECTORMESSENGER_HH
