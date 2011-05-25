
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// UserRunAction.hh
// 
// Defining actions performed at the beginning and/or the end of each run
//
/////////////////////////////////////////////////////////////////////////


#ifndef USERRUNACTION_HH
#define USERRUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"


// G4UserRunAction is the base class for defining user actions performed at 
// the beginning and/or the end of each run:

class UserRunAction : public G4UserRunAction {

 public:
   UserRunAction(G4String file) : fileName(file) { }
   ~UserRunAction() { }

   // G4UserRunAction has two methods, BeginOfRunAction and EndOfRunAction, 
   // which can be overloaded by the user to define specific actions performed
   // the beginning and the end of each run. 
   // Information about the run can be retrieved from the G4Run objects passed 
   // to these functions.
   virtual void BeginOfRunAction(const G4Run* run);
   virtual void EndOfRunAction(const G4Run* run);

 private:

   G4String fileName;
};

#endif // USERRUNACTION_HH
