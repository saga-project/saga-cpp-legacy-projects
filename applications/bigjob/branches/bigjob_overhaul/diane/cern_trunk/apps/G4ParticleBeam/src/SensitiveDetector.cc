/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// SensitiveDetector.cc
// 
// Definition of the detector as a sensitive volume
//
/////////////////////////////////////////////////////////////////////////


#include "SensitiveDetector.hh"
#include "DetectorHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "CLHEP/Random/Randomize.h"
#include "G4DynamicParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"

G4Allocator<DetectorHit> DetectorHitAllocator;


SensitiveDetector::SensitiveDetector(G4String SDuniqueName) :
    G4VSensitiveDetector(SDuniqueName),
    hitsCollectionID(-1) {

  //Constructor of the sensitive detector class. The same event 
  //could have more hit collections (e.g. output from different 
  //detectors), that are identified by their name.
  //Here one add "HitsCollection" to the list of names of existing 
  //collection. In this example, we have only one detector, so we 
  //define only one hit collection, which is named "HitsCollection"

  G4String hitsCollectionName = "HitsCollection";
  collectionName.insert(hitsCollectionName);
}


SensitiveDetector::~SensitiveDetector() {

}


void SensitiveDetector::Initialize(G4HCofThisEvent* hitsCollEvent) {
  //This method is called at each new event, and it gets the HCofThisEvents
  //for the event.
 
  //First, we have to create the hits-collection for the current event, 
  //with its name (collectionName[0]) and its corresponding Sensitive Detector.
  //Each defined hits-collection receives an ID (0,1,...) which can 
  //be then used to retrieve the information. Since we have only one 
  //hits-collection here, the ID will be "0".
  hitsCollection = new DetectorHitsCollection(SensitiveDetectorName, 
                                              collectionName[0]);
  
  //Here, retrieve the ID of the hits-collection called collectionName[0],
  //if it has not been done already. This ID is stored in the variable 
  //called hitsCollectionID.
  if(hitsCollectionID < 0) {
     hitsCollectionID =
         G4SDManager::GetSDMpointer() -> GetCollectionID(collectionName[0]);
  }
  
  //Finally, the hits collection just defined (with its ID and its hits) 
  //is registered to the HCOfThisEvent of the current event. As said before, many hits 
  //collections may be created for the same event (e.g. output from 
  //different detectors), and they all have to be registered.
  hitsCollEvent -> AddHitsCollection(hitsCollectionID, hitsCollection);
}


G4bool SensitiveDetector::ProcessHits(G4Step* step, 
                                      G4TouchableHistory*) {
  //This method is called when a particle goes through a sensitive 
  //detector. It possibly creates a new hit, and add its to the collection.
  //It eventually returns a boolean value. 
  //The arguments it gets are the G4Step and the G4TouchableHistory 
  //
  //The touchable history that we get in this method contains the full information about the 
  //genealogy of the current volume in the read-out geometry. One can use the touchable to 
  //access the information on the position/rotation of a given volume.

  if(step == 0) return false;

  //Retrieve the energy deposited from the step
  G4double energyDeposit = step -> GetTotalEnergyDeposit();

  G4StepPoint* preStepPoint = step -> GetPreStepPoint();
  G4StepPoint* postStepPoint = step -> GetPostStepPoint();

  const G4ThreeVector& preStepPointPosition = preStepPoint -> GetPosition();
  const G4ThreeVector& postStepPointPosition = postStepPoint -> GetPosition();

  G4double preStepPointZCoord = preStepPointPosition.z();
  G4double postStepPointZCoord = postStepPointPosition.z();

  G4double preStepPointYCoord = preStepPointPosition.y();
  G4double postStepPointYCoord = postStepPointPosition.y();

  G4double preStepPointXCoord = preStepPointPosition.x();
  G4double postStepPointXCoord = postStepPointPosition.x();

  G4double frac = CLHEP::RandFlat::shoot();

  G4double deltaZ = postStepPointZCoord - preStepPointZCoord; 
  G4double z = preStepPointZCoord + frac * deltaZ;

  G4double deltaY = postStepPointYCoord - preStepPointYCoord; 
  G4double y = preStepPointYCoord + frac * deltaY;
 
  G4double deltaX = postStepPointXCoord - preStepPointXCoord; 
  G4double x = preStepPointXCoord + frac * deltaX;

  G4double r = std::sqrt(x * x + y * y);

  G4Track* track = step -> GetTrack();
  const G4DynamicParticle* dynParticle = track -> GetDynamicParticle();
  G4ParticleDefinition* particle = dynParticle -> GetDefinition();
  G4String particleName = particle -> GetParticleName();


  //Now we create a new hit object and fill it with values to be stored
  DetectorHit* hit = new DetectorHit(); 
  hit -> SetEnergyDeposit(energyDeposit);
  hit -> SetZCoord(z);
  hit -> SetRCoord(r);

  //The hit is finally added to the corresponding hits-collection 
  //created in Initialize(), namely having the proper name and associated 
  //to the proper sensitive detector.
  hitsCollection -> insert(hit);

  return true;
}


void SensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
  //Here one could specify actions to be performed at the 
  //end of the event
}
