
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// DetectorHitsCollection.hh
//
// Type definition of hits collection (the template parameter of 
// G4THitsCollection is the DetectorHit class).
//
/////////////////////////////////////////////////////////////////////////

#ifndef DETECTORHITSCOLLECTION_HH
#define DETECTORHITSCOLLECTION_HH

#include "DetectorHit.hh"
#include "G4THitsCollection.hh"

typedef G4THitsCollection<DetectorHit> DetectorHitsCollection;

#endif // DETECTORHITSCOLLECTION_HH

