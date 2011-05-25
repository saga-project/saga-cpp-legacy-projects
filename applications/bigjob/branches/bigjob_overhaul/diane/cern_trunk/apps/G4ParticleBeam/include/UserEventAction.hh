
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// UserEventAction.hh
// 
// Defining actions performed at the beginning and/or the end of each event
//
/////////////////////////////////////////////////////////////////////////


#ifndef USEREVENTACTION_HH
#define USEREVENTACTION_HH

#include "G4Timer.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"


// G4UserEventAction is the base class for defining user actions performed at 
// the beginning and/or end of each event:

class UserEventAction : public G4UserEventAction {

 public:
   UserEventAction();
   ~UserEventAction();

   // G4UserEventAction has two methods, BeginOfEventAction and 
   // EndOfEventAction, which can be overloaded by the user to define specific 
   // actions performed the beginning and the end of each event.
   // Information about the event can be retrieved from the G4Event objects 
   // passed to these functions.
   virtual void BeginOfEventAction(const G4Event*);
   virtual void EndOfEventAction(const G4Event* event);

 private:

    // Hits ollection index:
   G4int hitsCollectionIndex;

   // Timer used to measure the cpu time of events
   G4Timer timerEvent;

   // Total CPU time for all processed events
   G4double cpuTime;
};

#endif // USEREVENTACTION_HH
