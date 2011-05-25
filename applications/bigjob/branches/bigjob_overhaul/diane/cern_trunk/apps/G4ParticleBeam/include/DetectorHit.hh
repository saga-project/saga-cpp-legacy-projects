
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// DetectorHit.hh
// 
// General purpose:
//  Represents a hit and contains relevant information about the hit.
//  (The user implementation should provide functionalities which allow to 
//  assign and retrieve hit information).
//  Hit objects can be created in the sensitive detector, and may be used
//  at the end of events to accumulate hit information.
//
// Purpose of the class in this example:
//  Represents a hit in the detector, where the relevant hit information is 
//  the energy deposited by the particle and the location (bin center) of 
//  the hit.
//
/////////////////////////////////////////////////////////////////////////


#ifndef DETECTORHIT_HH
#define DETECTORHIT_HH

#include <vector>
#include <utility>
#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "globals.hh"


// G4VHit is the abstract base class for creating hit objects:

class DetectorHit : public G4VHit {

 public:
   DetectorHit();
   DetectorHit(const DetectorHit& right);
   virtual ~DetectorHit();

   // Assignment and comparison operators:  
   const DetectorHit& operator= (const DetectorHit& right);
   int operator==(const DetectorHit& right) const;

   // The hit class has user-defined new and delete operators to speed up
   // the generation and deletion of hits objects:
   inline void* operator new(size_t);
   inline void operator delete(void* hit);

   // The G4VHit provides two methods, which can be overloaded by the user to
   // visualize hits or to print information about hits. Here, these methods 
   // are not used (dummy implementation):
   virtual void Draw() { }
   virtual void Print() { }

   // The current hit object can be used to keep track of the energy deposit 
   // and the the hit location (center of bins defined in the read-out 
   // geometry).
   // The following four methods allow to set and get the energy and location:
   inline void SetEnergyDeposit(G4double energy) {
     energyDeposit = energy;
   }
   inline void SetZCoord(G4double z) {
     zCoord = z;
   }
   inline void SetRCoord(G4double r) {
     rCoord = r;
   }
   inline G4double GetEnergyDeposit() {
     return energyDeposit;
   }
   inline G4double GetZCoord() {
     return zCoord;
   }
   inline G4double GetRCoord() {
     return rCoord;
   }

 private:

   // The energy deposit and hit location (bin center): 
   G4double energyDeposit;
   G4double zCoord;
   G4double rCoord;
};


extern G4Allocator<DetectorHit> DetectorHitAllocator;


// Implementation of the new operator:
inline void* DetectorHit::operator new(size_t) {

  void* hit;
  hit = (void*) DetectorHitAllocator.MallocSingle();
  return hit;
}


// Implementation of the delete operator:
inline void DetectorHit::operator delete(void* hit) {

  DetectorHitAllocator.FreeSingle((DetectorHit*) hit);
}

#endif // DETECTORHIT_HH
