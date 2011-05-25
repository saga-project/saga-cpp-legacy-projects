
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// DetectorConstruction.hh
// 
// Setup of the detector geometry, definition of dectector properties
//
// This class is mandatory for setting up Geant4 simulations.
//
/////////////////////////////////////////////////////////////////////////


#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class DetectorMessenger;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4VSolid;
class G4UserLimits;
class G4VisAttributes;


// G4VUserDetectorConstruction is the abstract base class for constructing
// the detector:

class DetectorConstruction : public G4VUserDetectorConstruction {
      
 public:
   DetectorConstruction(G4String detectorRegionName);
   virtual ~DetectorConstruction();

   // G4VUserDetectorConstruction has one pure virtual method which must be
   // implemented in derived classes: Construct()
   // This function is called by G4RunManager during the initialization.
   // The user must implement the detector and world geometry in the overloaded
   // Construct() function. The function must return the pointer to the
   // physical volume of the world. 
   virtual G4VPhysicalVolume* Construct();   

   // Following methods can be used (before a run starts) to change
   // the properties of the detector
   void SetBoxMaxStepSize(G4double max);
   void SetBoxColour(G4String colour);

 private:
   // Messenger for communication with UI
   DetectorMessenger* messenger;

   // Physical volume of the world and the detector
   G4VPhysicalVolume* worldVolPhys;
   G4VPhysicalVolume* detVolPhys;

   // Logical and solid volumes associated with the detector
   G4LogicalVolume* detVolLogic;
   G4VSolid* detVolSolid;
 
   // User limits and visualization attributes associated with the detector
   G4UserLimits* detVolUserLimits;
   G4VisAttributes* detVolVisAtt;

   // Detector region name (the detector is defined as a Geant4 region)
   G4String detRegionName;
};

#endif // DETECTORCONSTRUCTION_HH
