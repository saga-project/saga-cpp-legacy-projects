
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// DetectorHit.cc
// 
/////////////////////////////////////////////////////////////////////////


#include "DetectorHit.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"


DetectorHit::DetectorHit() :
   energyDeposit(0),
   zCoord(0),
   rCoord(0) {

}


DetectorHit::DetectorHit(const DetectorHit& right) :
   G4VHit(right),
   energyDeposit(right.energyDeposit),
   zCoord(right.zCoord),
   rCoord(right.rCoord) {

}


DetectorHit::~DetectorHit() {

}


const DetectorHit& DetectorHit::operator=(const DetectorHit& right) {

  energyDeposit = right.energyDeposit;
  zCoord = right.zCoord;
  rCoord = right.rCoord;
  

  return *this;
}


int DetectorHit::operator==(const DetectorHit& right) const {

  return (this == &right) ? 1 : 0;
}

