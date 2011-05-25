#ifndef AIDAOBJECTMANAGER_HH
#define AIDAOBJECTMANAGER_HH

/* Singleton class which manages all AIDA objects bound to a particular 
   filestream. Takes care about deleting objects in correct order when
   destructing the class.
*/

#include <AIDA/AIDA.h>
#include "globals.hh"


class AidaObjectManager {

 public:
   static AidaObjectManager* Instance(G4String fileNameBase = "");
   static void Destroy(); 

   AIDA::IHistogramFactory* GetHistogramFactory() { return histogramFactory; }

   AIDA::IHistogram1D* GetEnergyDepositHisto() { return energydepositHisto; }
   AIDA::IHistogram1D* GetEnergyDepositRadialHisto() 
                                         { return energydepositradialHisto; }

   inline G4String FileName() { return fileName; }

 protected:
   virtual ~AidaObjectManager();
   AidaObjectManager(G4String fileNameBase);

   AidaObjectManager(const AidaObjectManager& only);
   const AidaObjectManager& operator=(const AidaObjectManager& only);

 private:
   static AidaObjectManager* instance;

   G4String fileName;
   AIDA::IAnalysisFactory* analysisFactory;
   AIDA::ITree* tree;
   AIDA::IHistogramFactory* histogramFactory;

   AIDA::IHistogram1D* energydepositHisto;
   AIDA::IHistogram1D* energydepositradialHisto;
};

#endif // AIDAOBJECTMANAGER_HH
