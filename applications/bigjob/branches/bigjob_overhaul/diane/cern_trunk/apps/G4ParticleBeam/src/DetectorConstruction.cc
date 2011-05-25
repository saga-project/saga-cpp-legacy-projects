
/////////////////////////////////////////////////////////////////////////
// G4Tutorial:
//
// DetectorConstruction.cc
// 
/////////////////////////////////////////////////////////////////////////


#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "SensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4RegionStore.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

DetectorConstruction::DetectorConstruction(G4String detectorRegionName) : 
    messenger(0),
    worldVolPhys(0),
    detVolPhys(0),
    detVolLogic(0),
    detVolSolid(0),
    detVolUserLimits(0),
    detVolVisAtt(0),
    detRegionName(detectorRegionName) {

  // Instantiation of messenger
  messenger = new DetectorMessenger(this);
}


DetectorConstruction::~DetectorConstruction() {
 
  // Deleting detector messenger instance
  delete messenger;
}


G4VPhysicalVolume* DetectorConstruction::Construct() {

  // I. Setup of the world volume:
  //
  // The world is defined as a cube (dimension: 100 x 100 x 100 cm)
  G4VSolid* worldVolSolid = new G4Box("World", 50 * cm, 50 * cm, 50 * cm);
  
  // The world "material" is vacuum. Parameters used for the definition of 
  // the material "vacuum" are:
  // The material name:
  G4String nameVacuum = "Vacuum";     
  // The mole mass, density:
  G4double densityVacuum = universe_mean_density;
  G4double moleMassVacuum = 1.01 * g/mole;
  // The temperature, pressure and atomic number:
  G4double pressureVacuum = 3.e-18 * pascal;
  G4double temperatureVacuum = 2.73 * kelvin;
  G4double ZVacuum = 1.;

  // Constructing vacuum:
  G4Material* vacuum = new G4Material(nameVacuum, ZVacuum, moleMassVacuum,
		 densityVacuum, kStateGas, temperatureVacuum, pressureVacuum);

  // The next step is to create a logical world volume (By creating a logical 
  // volume, a material or special properties can be assigned to the 
  // geometrical object)
  G4LogicalVolume* worldVolLogic = 
                   new G4LogicalVolume(worldVolSolid, // geometrical object
                                       vacuum,   // material
                                       "World");      // name

  // Visualization: The world should be invisible 
  worldVolLogic -> SetVisAttributes(G4VisAttributes::Invisible);

  // Finally a physical object of the world must be created (By creating a
  // physical volume, logical volumes are placed within the coordinate system
  // and a volume hierarchy can be established)
  worldVolPhys = new G4PVPlacement(0,               // no rotation
                                   G4ThreeVector(), // no translation
                                   "World",         // name 
                                   worldVolLogic,   // the logical volume
                                   0,               // the world has no mother
                                   false,           // param. for future use
                                   0);              // copy number

  // II. Setup of the detector volume (placed within the world volume):
  //
  // The detector is defined as a cube (dimension: 4 x 4 x 4 cm)
  G4double boxLength = 4.0 * cm; 
 
  detVolSolid = new G4Box("Detector", 
                          boxLength * 0.5, boxLength * 0.5, boxLength * 0.5); 
      


  G4double a = 1.0794 * g/mole;
  G4Element* elH = new G4Element ("Hydrogen","H",1.,a);

  a = 15.9994 * g/mole;
  G4Element* elO = new G4Element("Oxygen","O",8.,a);

  G4Material* water = new G4Material("Water", 1.000*g/cm3, 2);
  water -> AddElement(elH,2);
  water -> AddElement(elO,1);
  water -> GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

  // The detector material is silicon. Parameters for constructing silicon:
  G4String  nameSilicon = "Silicon";
  // The mole mass, atomic number, density and the mean excitation energy:
  G4double moleMassSilicon = 28.085 * g/mole;
  G4double ZSilicon = 14;
  G4double densitySilicon = 2.33 *g/cm3;
  G4double ISilicon = 173.0 * eV;
  G4Material* silicon = new G4Material(nameSilicon, ZSilicon, moleMassSilicon, 
                                       densitySilicon);
  silicon -> GetIonisation() -> SetMeanExcitationEnergy(ISilicon);

  // Definition of the logical volume of the detector
  detVolLogic = new G4LogicalVolume(detVolSolid,  // geometrical object
                                    water,    // material
                                    "Detector");  // name

  // Visulization: The detector should be visible (colour: yellow). To achieve
  // this, a G4VisAttributes object must be instantiated and assigned to the 
  // logical volume  
  detVolVisAtt = new G4VisAttributes(true,                 // visibility: true
                                     G4Colour::Red());  // colour: yellow
  detVolLogic -> SetVisAttributes(detVolVisAtt);


  // The maximum step size of particles in the detector should be limited. For
  // this purpose a G4UserLimits object must be instantiated and assigned to
  // the logical volume (The max. step size is set to 0.001 mm).
  detVolUserLimits = new G4UserLimits(0.001 * mm);
  detVolLogic -> SetUserLimits(detVolUserLimits);

  // Definition of the physical volume of the detector
  detVolPhys = new G4PVPlacement(0,    // no rotation
	             G4ThreeVector(0, 0, boxLength * 0.5), // translation
                     "Detector",      // name
                     detVolLogic,     // logical volume of the detector
                     worldVolPhys,    // physical volume of mother volume
                     false,           // param. for future use
                     0);              // copy number

  // The detector is defined as a Geant4 region
  G4Region* detectorRegion = new G4Region(detRegionName);
  detectorRegion -> AddRootLogicalVolume(detVolLogic);

  // So far, the detector created above is not yet a real detector: It is just
  // a geometrical object (with some attributes) placed within the world 
  // volume.
  // To make the volume a detector, which can record e.g. hits, one must 
  // define a sensitive volume associated with it (see 
  // DetectorSensitiveVolume.hh/.cc for details on the definition of a
  // sensitive detector). 
  // For this purpose, a DetectorSensitiveVolume object is instantiated
  G4VSensitiveDetector* detVolSD = new SensitiveDetector("Detector"); 

  // The sensitive volume is then assigned to the detector
  detVolLogic -> SetSensitiveDetector(detVolSD);

  // Finally, the sensitive detector manager must be informed about this new 
  // sensitive detector
  G4SDManager::GetSDMpointer() -> AddNewDetector(detVolSD);






  // The function must return the physical volume of the world
  return worldVolPhys;
}


void DetectorConstruction::SetBoxMaxStepSize(G4double max) {

  if(max > 0.0 * mm) {
     detVolUserLimits -> SetMaxAllowedStep(max);
     G4RunManager::GetRunManager() -> DefineWorldVolume(worldVolPhys);
  } 
}


void DetectorConstruction::SetBoxColour(G4String colour) {

  G4Colour col;
  if(G4Colour::GetColour(colour,col)) {
     detVolVisAtt -> SetColour(col);
     G4RunManager::GetRunManager() -> DefineWorldVolume(worldVolPhys);
  } 
}


