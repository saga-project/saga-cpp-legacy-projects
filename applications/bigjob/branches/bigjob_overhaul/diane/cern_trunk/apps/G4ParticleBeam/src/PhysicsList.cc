

#include "PhysicsList.hh"
#include "G4LeptonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ParticleDefinition.hh"

#include "G4ProcessManager.hh"
#include "G4Decay.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4MultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4MuPairProduction.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4hMultipleScattering.hh"
#include "G4ionIonisation.hh"
#include "G4hIonisation.hh"
#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4BinaryCascade.hh"
#include "G4PiMinusAbsorptionAtRest.hh"
#include "G4PiNuclearCrossSection.hh" 
#include "G4StepLimiter.hh"
#include "G4ProtonInelasticProcess.hh" 
#include "G4TripathiCrossSection.hh"
#include "G4IonsShenCrossSection.hh"
#include "G4DeuteronInelasticProcess.hh"
#include "G4TritonInelasticProcess.hh"
#include "G4AlphaInelasticProcess.hh"
#include "G4BinaryLightIonReaction.hh"
#include "G4LEDeuteronInelastic.hh"
#include "G4LETritonInelastic.hh"
#include "G4LEAlphaInelastic.hh"
#include "G4LElastic.hh"
#include "G4HadronElasticProcess.hh"

#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Alpha.hh"
#include "G4Triton.hh"
#include "G4Deuteron.hh"
#include "G4He3.hh"
#include "G4GenericIon.hh"

#include "G4RegionStore.hh"
#include "G4Region.hh"


PhysicsList::PhysicsList(G4String detRegName) :
    detectorRegionName(detRegName) {

  // Initialization of value for lower production threshold (the data member 
  // defaultCutValue is defined in the parent class) 
  defaultCutValue = 0.001 * mm;   
}

PhysicsList::~PhysicsList() {


}

void PhysicsList::ConstructParticle() {

  G4LeptonConstructor lepton;
  lepton.ConstructParticle();

  G4BosonConstructor boson;
  boson.ConstructParticle();

  G4MesonConstructor meson;
  meson.ConstructParticle();

  G4BaryonConstructor baryon;
  baryon.ConstructParticle();

  G4ShortLivedConstructor shortLived;
  shortLived.ConstructParticle();

  G4IonConstructor ion;
  ion.ConstructParticle();
}


void PhysicsList::ConstructProcess() {

  AddTransportation();

  G4ParticleDefinition* particle = 0;
  G4ProcessManager* pmanager = 0;

  // ************************************
  // A. Decay
  // ************************************

  theParticleIterator -> reset();

  while( (*theParticleIterator)() ) {
    G4ParticleDefinition* particle = theParticleIterator -> value();

    if(!(particle -> IsShortLived())) {
	  G4String name =  particle -> GetParticleName();

          G4Decay* decayProcess = new G4Decay();
          if(decayProcess -> IsApplicable(*particle)) {

             pmanager = particle -> GetProcessManager();
	     pmanager -> AddProcess(decayProcess);
	 
	     pmanager -> SetProcessOrdering(decayProcess, idxPostStep);
	     pmanager -> SetProcessOrdering(decayProcess, idxAtRest);
          }
    }
  }

  // ************************************
  // B. Electromagnetic Physics
  // ************************************

  // *************
  // *** Gamma ***
  // *************

  particle = G4Gamma::Definition(); 
  pmanager = particle->GetProcessManager();

  pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());
  pmanager->AddDiscreteProcess(new G4ComptonScattering());
  pmanager->AddDiscreteProcess(new G4GammaConversion());

  // ****************
  // *** Electron ***
  // ****************

  particle = G4Electron::Definition();
  pmanager = particle->GetProcessManager();

  pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
  pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
  pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);

  // ****************
  // *** Positron ***
  // ****************

  particle = G4Positron::Definition();
  pmanager = particle->GetProcessManager();

  pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
  pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
  pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
  pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);

  // *************
  // *** Muon+ ***
  // *************

  particle = G4MuonPlus::Definition();
  pmanager = particle->GetProcessManager();

  pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
  pmanager->AddProcess(new G4MuIonisation,       -1, 2, 2);
  pmanager->AddProcess(new G4MuBremsstrahlung,   -1, 3, 3);
  pmanager->AddProcess(new G4MuPairProduction,   -1, 4, 4);       

  // *************
  // *** Muon- ***
  // *************

  particle = G4MuonMinus::Definition();
  pmanager = particle->GetProcessManager();

  pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
  pmanager->AddProcess(new G4MuIonisation,       -1, 2, 2);
  pmanager->AddProcess(new G4MuBremsstrahlung,   -1, 3, 3);
  pmanager->AddProcess(new G4MuPairProduction,   -1, 4, 4);       

  // *********************
  // *** Hadrons, Ions ***
  // *********************

  theParticleIterator -> reset();

  while( (*theParticleIterator)() ){

    particle = theParticleIterator->value();
    pmanager = particle->GetProcessManager();

    if(particle == G4Alpha::Definition() ||
       particle == G4He3::Definition() ||
       particle == G4Triton::Definition() ||
       particle == G4Deuteron::Definition() ||
       particle == G4GenericIon::Definition() ) {

      pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4ionIonisation,      -1, 2, 2);
    }
    else if((!particle->IsShortLived()) &&
            (particle->GetPDGCharge() != 0.0) && 
            (particle->GetParticleName() != "chargedgeantino") &&
            particle != G4Electron::Definition() &&
            particle != G4Positron::Definition() &&
            particle != G4MuonPlus::Definition() &&
            particle != G4MuonMinus::Definition()) {

      pmanager->AddProcess(new G4hMultipleScattering,-1,  1, 1);
      pmanager->AddProcess(new G4hIonisation,        -1,  2, 2); 
      pmanager->AddProcess(new G4StepLimiter,        -1, -1, 3);
    }
  }
 
  // ************************************
  // C. Hadronic Physics (elastic)
  // ************************************

  // ***************************************************
  // *** Proton, Neutron, Pion plus, Pion minus      ***
  // *** Deuteron, Triton, Alpha, He3, Generic Ion   ***
  // ***************************************************
 
  // This is the elastic process to be used for all particles but hadrons. 
  // We'll register the G4LElastic model in the full energy range. 
  G4HadronElasticProcess* hadronIonElasticProcess = 
                           new G4HadronElasticProcess("elastic-ionhadron");

  // Define G4LElastic model for all hadrons. It is applied to the full energy 
  // range (0 - 100 TeV), by default
  G4LElastic* hadronIonLElasticModel = new G4LElastic();
  
  hadronIonElasticProcess -> RegisterMe(hadronIonLElasticModel); 

  particle = G4Proton::Proton();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess); 

  particle = G4PionPlus::PionPlus(); 
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess);

  particle = G4PionMinus::PionMinus();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess); 

  particle = G4Deuteron::Deuteron();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess); 

  particle = G4Triton::Triton();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess);

  particle = G4Alpha::Alpha();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess); 

  particle = G4He3::He3();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess); 

  particle = G4GenericIon::GenericIon();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(hadronIonElasticProcess);


  // ************************************
  // D. Hadronic Physics (Inelastic)
  // ************************************

  // ************************************
  // *** Pion+/- (common definitions) ***
  // ************************************

  G4double pionBinaryMinEnergy = 0. * MeV;
  G4double pionBinaryMaxEnergy = 1.2 * GeV;

  G4BinaryCascade* pionBinaryCascadeModel = new G4BinaryCascade();
  pionBinaryCascadeModel -> SetMinEnergy(pionBinaryMinEnergy);
  pionBinaryCascadeModel -> SetMaxEnergy(pionBinaryMaxEnergy);

  G4PiNuclearCrossSection* pionNuclearCrossSection = 
                  new G4PiNuclearCrossSection();

  // *************
  // *** Pion+ ***
  // *************

  G4PionPlusInelasticProcess* pionPlusInelasticProcess = 
                  new G4PionPlusInelasticProcess("inelastic-pionplus");

  pionPlusInelasticProcess -> AddDataSet(pionNuclearCrossSection);
  pionPlusInelasticProcess -> RegisterMe(pionBinaryCascadeModel);
 
  particle = G4PionPlus::PionPlus(); 
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(pionPlusInelasticProcess);
 
  // **************
  // *** Pion-  ***
  // **************

  G4PionMinusInelasticProcess* pionMinusInelasticProcess = 
                     new G4PionMinusInelasticProcess("inelastic-pionminus");
  G4PiMinusAbsorptionAtRest* pionMinusAbsAtRestProcess = 
                     new G4PiMinusAbsorptionAtRest();

  pionMinusInelasticProcess -> AddDataSet(pionNuclearCrossSection);
  pionMinusInelasticProcess -> RegisterMe(pionBinaryCascadeModel);

  particle = G4PionMinus::PionMinus();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(pionMinusInelasticProcess);
  pmanager -> AddRestProcess(pionMinusAbsAtRestProcess);


  // **************
  // *** Proton ***
  // **************

  //For proton inelastic process, we use only one model, G4BinaryCascade, 
  // between 0 and 10 GeV. We instantiate this model here, and then 
  // we'll register to the process.
  G4double protonBinaryMinEnergy = 0. * MeV;
  G4double protonBinaryMaxEnergy = 10. * GeV;
  G4BinaryCascade* protonBinaryCascadeModel = new G4BinaryCascade();
  protonBinaryCascadeModel -> SetMinEnergy(protonBinaryMinEnergy);
  protonBinaryCascadeModel -> SetMaxEnergy(protonBinaryMaxEnergy);
  
  //This is the process handling proton inelastic interactions. It is a 
  // specific process called G4ProtonInelasticProcess. See list at:
  // http://geant4.cern.ch/support/proc_mod_catalog/processes/
  G4ProtonInelasticProcess* protonInelasticProcess = 
                           new G4ProtonInelasticProcess("inelastic-proton");

  //Register the model to the proton process
  protonInelasticProcess -> RegisterMe(protonBinaryCascadeModel);
   
  //Retrieve the proton process manager, and register the inelastic process
  // to it.
  particle = G4Proton::Proton();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(protonInelasticProcess);

  // ****************************************************
  // *** Deuteron, Triton, Alpha (common definitions) ***
  // ****************************************************

  G4double ionLEPMaxEnergy = 100. * MeV;
  G4double ionBinaryMinEnergy = 99. * MeV;
  G4double ionBinaryMaxEnergy = 10. * GeV;
 
  G4BinaryLightIonReaction* ionBinaryModel = new G4BinaryLightIonReaction;
  ionBinaryModel -> SetMinEnergy(ionBinaryMinEnergy);
  ionBinaryModel -> SetMaxEnergy(ionBinaryMaxEnergy);

  G4TripathiCrossSection* ionTripathiCrossSection = new G4TripathiCrossSection;
  G4IonsShenCrossSection* ionShenCrossSection = new G4IonsShenCrossSection;

  // ****************
  // *** Deuteron ***
  // ****************

  G4DeuteronInelasticProcess* deuteronInelasticProcess = 
                       new G4DeuteronInelasticProcess("inelastic-deuteron");

  G4LEDeuteronInelastic* deuteronLEPModel = new G4LEDeuteronInelastic;
  deuteronLEPModel -> SetMaxEnergy(ionLEPMaxEnergy);

  deuteronInelasticProcess -> AddDataSet(ionTripathiCrossSection);
  deuteronInelasticProcess -> AddDataSet(ionShenCrossSection);
  deuteronInelasticProcess -> RegisterMe(deuteronLEPModel);
  deuteronInelasticProcess -> RegisterMe(ionBinaryModel);

  particle = G4Deuteron::Deuteron();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(deuteronInelasticProcess);


  // **************
  // *** Triton ***
  // **************

  G4TritonInelasticProcess* tritonInelasticProcess = 
                              new G4TritonInelasticProcess("inelastic-triton");
  
  G4LETritonInelastic* tritonLEPModel = new G4LETritonInelastic;
  tritonLEPModel -> SetMaxEnergy(ionLEPMaxEnergy);

  tritonInelasticProcess -> AddDataSet(ionTripathiCrossSection);
  tritonInelasticProcess -> AddDataSet(ionShenCrossSection);
  tritonInelasticProcess -> RegisterMe(tritonLEPModel);
  tritonInelasticProcess -> RegisterMe(ionBinaryModel);

  particle = G4Triton::Triton();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(tritonInelasticProcess);
 

  // *************
  // *** Alpha ***
  // *************

  G4AlphaInelasticProcess* alphaInelasticProcess = 
                              new G4AlphaInelasticProcess("inelastic-alpha");

  G4LEAlphaInelastic* alphaLEPModel = new G4LEAlphaInelastic;
  alphaLEPModel -> SetMaxEnergy(ionLEPMaxEnergy);
 
  alphaInelasticProcess -> AddDataSet(ionTripathiCrossSection);
  alphaInelasticProcess -> AddDataSet(ionShenCrossSection);
  alphaInelasticProcess -> RegisterMe(alphaLEPModel);
  alphaInelasticProcess -> RegisterMe(ionBinaryModel);

  particle = G4Alpha::Alpha();
  pmanager = particle -> GetProcessManager();
  pmanager -> AddDiscreteProcess(alphaInelasticProcess); 
}

void PhysicsList::SetCuts() {

  G4double lowerProdLimit = 250.0 * eV;
  G4double upperProdLimit = 1.0 * GeV;
  G4ProductionCutsTable::GetProductionCutsTable()
                           -> SetEnergyRange(lowerProdLimit, upperProdLimit);

  // The default cut value, given by the data member defaultCutValue, is set 
  // for all particles (electrons, positrons, photons) for the default region, 
  // which is the world region:
  SetCutsWithDefault();

  // To define production cuts for the detector, a G4ProductionCuts object is
  // first instantiated and the lower cut value is set:
  G4ProductionCuts* prodCutsDetector = new G4ProductionCuts;
  prodCutsDetector -> SetProductionCut(defaultCutValue);

  // The next step is to retrieve the G4Region instance, which represents the
  // detector, from the region store (by using the detector region name, which
  // was stored in a data member of the current class):
  G4Region* detectorRegion = 
               G4RegionStore::GetInstance() -> GetRegion(detectorRegionName);

  // Then the G4ProductionCuts instance is assigned to the detector region:
  // (check that detectorRegion is a valid pointer before, otherwise the 
  // program will crash)
  if (detectorRegion)
    detectorRegion -> SetProductionCuts(prodCutsDetector);

  // The production cuts are finally printed:
  DumpCutValuesTable();
}
