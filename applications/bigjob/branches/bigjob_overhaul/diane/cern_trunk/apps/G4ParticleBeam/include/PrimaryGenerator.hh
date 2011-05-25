
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// PrimaryGenerator.hh
// 
// Definition of primary particles and their properties (particle types, 
// spectrum of energies, initial vertices, ...).
//
// This class is mandatory for setting up Geant4 simulations.
//
/////////////////////////////////////////////////////////////////////////


#ifndef PRIMARYGENERATOR_HH
#define PRIMARYGENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4Event;
class G4ParticleGun;
class PrimaryGeneratorMessenger;


// G4VUserPrimaryGeneratorAction is the abstract base class for primary 
// particle generators:

class PrimaryGenerator : public G4VUserPrimaryGeneratorAction {

 public:
   PrimaryGenerator();
   ~PrimaryGenerator();

   // G4VUserPrimaryGeneratorAction has one pure virtual method which must
   // be implemented in derived classes: GeneratePrimaries()
   // This function is called by G4RunManager each time an event is processed 
   // in the event loop.
   // GeneratePrimaries is not meant to create the primary vertex/particle by
   // itself, but the user should use a concrete implementation of 
   // G4VPrimaryGenerator (such as G4ParticleGun), which takes care of that.
   void GeneratePrimaries(G4Event*);

   // Following methods can be called from the messenger to change the 
   // properties of the primary generator:
   void SetPrimaryKineticEnergy(G4double);
   void SetSigmaKineticEnergy(G4double);
   void SetIncidentAngle(G4double);
   void SetParticleType(G4String);

 private:

   // Particle gun: Concrete implementation of G4VPrimaryGenerator used to 
   // generate primary particles and vertices:
   G4ParticleGun* particleGun;

   // Messenger for communication with UI:
   PrimaryGeneratorMessenger* messenger;

   // Parameters describing some properties of primary particles (see the
   // PrimaryGenerator.cc file for details):
   G4double primaryKineticEnergy;
   G4double sigmaKineticEnergy;
   G4double incidentAngle;
};

#endif // PRIMARYGENERATOR_HH
