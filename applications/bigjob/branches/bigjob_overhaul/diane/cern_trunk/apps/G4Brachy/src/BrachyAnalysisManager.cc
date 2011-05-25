//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// Code developed by:
//  S.Guatelli
//
//    *******************************
//    *                             *
//    *    BrachyAnalysisManager.cc *
//    *                             *
//    *******************************
//
// $Id: BrachyAnalysisManager.cc,v 1.2 2008/10/17 19:56:57 moscicki Exp $
// GEANT4 tag $Name:  $
//
#ifdef  G4ANALYSIS_USE
#include <stdlib.h>
#include <fstream>
#include "BrachyAnalysisManager.hh"

#include "G4ios.hh"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "AIDA/IManagedObject.h"
#include "AIDA/IAnalysisFactory.h"
#include "AIDA/IHistogramFactory.h"
#include "AIDA/ITupleFactory.h"
#include "AIDA/ITreeFactory.h"
#include "AIDA/ITree.h"
#include "AIDA/ITuple.h"

# include <iostream>

BrachyAnalysisManager* BrachyAnalysisManager::instance = 0;

BrachyAnalysisManager::BrachyAnalysisManager() : 
  aFact(0), theTree(0), histFact(0), h1(0),h2(0)
  
{
  //build up  the  factories
  aFact = AIDA_createAnalysisFactory();

  AIDA::ITreeFactory *treeFact = aFact->createTreeFactory(); 
  
  G4String fileName = "brachytherapy.xml";
 
// FOR SOME REASON COMPRESSING HISTOS PRODUCES CORRUPTED GZIP

//   if(getenv("G4BRACHY_COMPRESS_HISTOGRAMS"))
//     {
//       std::cout << "Creating COMPRESSED histogram " << fileName << "******************" << std::endl;
//       theTree = treeFact->create(fileName,"xml",false, true);
//     }
//   else
//     {
      std::cout << "Creating histogram " << fileName << "******************" << std::endl;
      theTree = treeFact->create(fileName,"xml",false, true,"uncompressed");
//     }
  
  delete treeFact;
}

BrachyAnalysisManager::~BrachyAnalysisManager() 
{ 
  delete histFact;
  histFact = 0;

  delete theTree;
  histFact = 0;

  delete aFact;
  aFact = 0;
}

BrachyAnalysisManager* BrachyAnalysisManager::getInstance()
{
  if (instance == 0) instance = new BrachyAnalysisManager;
  return instance;
}

void BrachyAnalysisManager::book() 
{ 
  histFact = aFact->createHistogramFactory( *theTree );
  
  //creating a 1D histogram ...
  h1 = histFact->createHistogram2D("10","Energy, pos", //histoID,histo name
				    40 ,-20.,20.,   //bins'number,xmin,xmax 
                                    40,-20.,20.    );//bins'number,ymin,ymax 
  //creating a 2D histogram ...
  h2 = histFact->createHistogram1D("20","Initial Energy", //histoID,histo name 
				  25,0.,0.05); //bins' number, xmin, xmax
  
  h3 = histFact->createHistogram1D("30","Dose Distribution", 
				  40,-20.,20.); //bins' number, xmin, xmax
}
 
void BrachyAnalysisManager::FillHistogramWithEnergy(G4double x,
                                                    G4double z, 
                                                    G4double energyDeposit)
{
  //2DHistrogram: energy deposit in a voxel which center is fixed in position (x,z)  
  h1->fill(x,z,energyDeposit);
}

void BrachyAnalysisManager::PrimaryParticleEnergySpectrum(G4double primaryParticleEnergy)
{
  //1DHisotgram: energy spectrum of primary particles  
  h2->fill(primaryParticleEnergy);
}
void BrachyAnalysisManager::DoseDistribution(G4double x,G4double energy)
{
  //1DHisotgram: energy spectrum of primary particles  
  h3->fill(x, energy);
}

void BrachyAnalysisManager::finish() 
{  
  // write all histograms to file ...
  theTree->commit();

  // close (will again commit) ...
  theTree->close();
}
#endif











