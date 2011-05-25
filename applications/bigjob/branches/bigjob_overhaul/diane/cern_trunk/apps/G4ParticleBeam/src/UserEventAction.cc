
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// UserEventAction.cc
// 
// Defining actions performed at the beginning and/or the end of each event
//
/////////////////////////////////////////////////////////////////////////


#include "UserEventAction.hh"
#include "DetectorHitsCollection.hh"
#include "AidaObjectManager.hh"
#include "G4Event.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"


UserEventAction::UserEventAction() :
   hitsCollectionIndex(-1),
   cpuTime(0) {

}


UserEventAction::~UserEventAction() {

}


void UserEventAction::BeginOfEventAction(const G4Event*) {

  // The timer is started at the beginning of an event
  timerEvent.Start();

  //Retrieve the ID of the hits collection named "HitsCollection".
  //In this case, this is the unique hit collection contained in 
  //the HCofThisEvent, but in principle there could be more (each 
  //with its name and its ID). This is done only at the very first 
  //event.
  if(hitsCollectionIndex < 0) {
    hitsCollectionIndex = G4SDManager::GetSDMpointer() 
                             -> GetCollectionID("HitsCollection");
  }
}


void UserEventAction::EndOfEventAction(const G4Event* event) {


  AIDA::IHistogram1D* energydepositHisto =
             AidaObjectManager::Instance() -> GetEnergyDepositHisto();

  AIDA::IHistogram1D* energydepositradialHisto =
             AidaObjectManager::Instance() -> GetEnergyDepositRadialHisto();

  //This method specifies the actions that must be performed at the 
  //end of each event (e.g. retrieve information, score, clean up 
  //things, etc.)

  //The following line verifies that a valid hits colletion has been 
  //found. If not, nothing happens.
  if(hitsCollectionIndex < 0) return;
      
  //Now, get the HCofThisEvent: it contains all the hits collections
  //that have been defined (one hit collection may be associated to 
  //each detector).
  G4HCofThisEvent* HCE = event -> GetHCofThisEvent();
 
  //Among all the HCs defined for the events, retrieve the one having 
  //ID=hitsCollectionIndex. Incidentally, this is the only hit collection
  //defined, but in principle they could be more. All of them can be 
  //retrieved in the same way.
  DetectorHitsCollection* hitsCollection = 0;
  if(HCE) hitsCollection = 
                (DetectorHitsCollection*)(HCE -> GetHC(hitsCollectionIndex));

  //Ok, now we have the hit collection at hand. If it is not a NULL pointer,
  //we can have a look at it, and read the information we need.
  if(hitsCollection) {
    //read the number of hits contained in the collection
    int numberHits = hitsCollection -> entries();

    //we can loop and get each single hit
    for(int i = 0; i < numberHits ; i++) {
       //retrieve the i-th hit from the collection.
       DetectorHit* hit = (*hitsCollection)[i];

       //get the information stored in the hit (position and energy)
       G4double zPosition = ( hit -> GetZCoord() )/mm;
       G4double rPosition = ( hit -> GetRCoord() )/mm;
       G4double energyDeposit = ( hit -> GetEnergyDeposit() ) / MeV;

       //use this information for scoring
       energydepositHisto -> fill(zPosition, energyDeposit);

       if(zPosition >= 20.0 * mm && zPosition <= 21.0 * mm)
          energydepositradialHisto -> fill(rPosition, energyDeposit);
    }
  }

  // Visualization: Trajectories are plotted at the end of each event if
  // G4VVisManager was defined
  if(G4VVisManager::GetConcreteInstance()) {

     G4HCofThisEvent* hitsCollEvent = event -> GetHCofThisEvent();
     G4int nmbHitsCollections = 
              G4SDManager::GetSDMpointer() -> GetCollectionCapacity();

     for(G4int i=0; i < nmbHitsCollections; i++) {
       //invoke the method DrawAllHits() of each single hit collection.
       //This method invokes the Draw() method of each single hit.
        DetectorHitsCollection* hitsCollection =
 	        (DetectorHitsCollection*) hitsCollEvent -> GetHC(i);

        if(hitsCollection) hitsCollection -> DrawAllHits();
     }
     //One here retrieves the trajectory container from the event, extract trajectories
     //and draws them all
     G4TrajectoryContainer* trajContainer = event -> GetTrajectoryContainer();

     G4int nmbTrajectories = 0;
     //get the number of stored trajectories 
     if (trajContainer) nmbTrajectories = trajContainer -> entries();
 
     //loop on the stored trajectories, get them one by one, and draw
     for(G4int i=0; i < nmbTrajectories; i++) { 
	//get the i-th trajectory stored in the TrajectoryContainer
        G4Trajectory* trajectory = 
           (G4Trajectory*) ((*(event->GetTrajectoryContainer()))[i]);
        //the following line draws the i-th trajectory. The argument (50) is used as a marker size
        if(trajectory) trajectory -> DrawTrajectory(50);
    }
  }

  // The timer is stopped at the end of an event
  timerEvent.Stop();

  // The CPU time for the current event is added to the total CPU time
  cpuTime += timerEvent.GetUserElapsed() + timerEvent.GetSystemElapsed();

  // The number of events is retrieved from the G4Event object
  // Remember: the first event has number 0, that´s why one adds 1
  G4int nmbEvents = event -> GetEventID() + 1;

  // The average CPU time is printed after each 5000 events
  if(!(nmbEvents % 5000)) {
     G4cout << "INFORMATION: " << nmbEvents << " events processed." 
            << G4endl
            << "INFORMATION: Average CPU time per event "
            << "(user + system elapsed) = " 
            << cpuTime/(G4double(nmbEvents)) 
            << " sec"
            << G4endl;
  }
}
