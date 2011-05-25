
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// PrimaryGeneratorMessenger.hh
// 
// Messenger for changing properties associated with the primary particle/
// vertex generator
//
/////////////////////////////////////////////////////////////////////////


#ifndef PRIMARYGENERATORMESSENGER_HH
#define PRIMARYGENERATORMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGenerator;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;


class PrimaryGeneratorMessenger : public G4UImessenger {

 public:
   PrimaryGeneratorMessenger(PrimaryGenerator*);
   ~PrimaryGeneratorMessenger();

   void SetNewValue(G4UIcommand*, G4String);

 private:
   PrimaryGenerator* primaryGenerator;

   G4UIdirectory* sourceDirectory;
   G4UIcmdWithADoubleAndUnit* primEnergyCmd;
   G4UIcmdWithAString* primParticleCmd;
   G4UIcmdWithADoubleAndUnit* sigmaEnergyCmd;
   G4UIcmdWithADoubleAndUnit* incidAngleCmd;
};

#endif // PRIMARYGENERATORMESSENGER_HH
