
#include "AidaObjectManager.hh"
#include <AIDA/AIDA.h>


AidaObjectManager* AidaObjectManager::instance = 0;


AidaObjectManager* AidaObjectManager::Instance(G4String fileNameBase) {
 
  if(instance == 0) {

    G4cout << "INFORMATION: Creating AidaObjectManager. File name = " 
           << fileNameBase << G4endl;

    instance = new AidaObjectManager(fileNameBase);
  }
  
  return instance;
}


void AidaObjectManager::Destroy() {

  G4cout << "INFORMATION: Destructing AidaObjectManager" << G4endl;

  if(!instance == 0) {

     delete instance;
     instance = 0;
  }
}


AidaObjectManager::AidaObjectManager(G4String fileNameBase) :
   fileName(fileNameBase),
   analysisFactory(0),
   tree(0),   
   histogramFactory(0), 
   energydepositHisto(0),
   energydepositradialHisto(0) {

  analysisFactory = AIDA_createAnalysisFactory();

  AIDA::ITreeFactory* treeFactory = analysisFactory -> createTreeFactory();

  if(fileName.empty()) {
     fileName = "sim.xml";
  }

  G4cout << "INFORMATION: AidaObjectManager: Creating output file " 
            << fileName  << G4endl;  

  tree = treeFactory -> create(fileName,"XML",false,true,"uncompressed"); 
  delete treeFactory;

  histogramFactory = analysisFactory -> createHistogramFactory(*tree); 

  G4cout << "UserEventAction: Creating Histogram \"energydeposit\"." << G4endl;
  energydepositHisto = 
        histogramFactory->createHistogram1D("energydeposit","Longitudinal energy deposition",400,0.0,40.0*mm);

  G4cout << "UserEventAction: Creating Histogram \"energydepositradial\"." << G4endl;
  energydepositradialHisto = 
        histogramFactory->createHistogram1D("energydepositradial","Radial energy deposition at depth of 20 mm",30,0.0,3.0*mm);
  
}


AidaObjectManager::~AidaObjectManager() {

  tree -> commit();
  tree -> close();

  G4cout << "INFORMATION: AidaObjectManager: File " 
            << fileName 
            << " closed." << G4endl;

  delete histogramFactory;
  delete tree;
  delete analysisFactory;
}

