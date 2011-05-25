#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

#include "G4VUserPhysicsList.hh"
#include "globals.hh"


class PhysicsList : public G4VUserPhysicsList {

 public:
   PhysicsList(G4String regionName);
   ~PhysicsList();


 protected:
   // Each particle type will be instantiated
   // This method is invoked by the RunManger 
   virtual void ConstructParticle();
 
   // Each physics process will be instantiated and
   // registered to the process manager of each particle type 
   // This method is invoked in Construct" method 
   virtual void ConstructProcess();

   //  "SetCuts" method sets a cut value for all particle types 
   //   in the particle table
   virtual void SetCuts(); 

 private:

   // Name of the detector region (region to which production cuts are applied)
   G4String detectorRegionName;
};

#endif
