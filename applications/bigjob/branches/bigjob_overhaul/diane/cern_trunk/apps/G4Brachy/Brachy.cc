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
//
// $Id: Brachy.cc
// GEANT4 tag $Name:  $
//
// --------------------------------------------------------------
//                 GEANT 4 - Brachytherapy example
// --------------------------------------------------------------
//
// Code developed by:
// S. Agostinelli, F. Foppiano, S. Garelli , M. Tropeano, S.Guatelli

//
//    *******************************
//    *                             *
//    *    Brachy.cc                *
//    *                             *
//    *******************************
//
// Brachytherapy simulates the energy deposition in a cubic (30*cm)
//
// brachytherapy source.
//
// Simplified gamma generation is used.
// Source axis is oriented along Z axis. The source is in the centre
//of the box.


#include "BrachySimulation.hh"
#include <cstdlib>


int main(int argc, char** argv){

  if(argc < 2 || argc > 3) {
      G4cerr << "ERROR Wrong number of arguments" 
             << G4endl;
      return -1;
  }

  G4String macrofile;

  BrachySimulation *simulation = new BrachySimulation(0);  

  simulation -> initialize(argc,argv);

  macrofile = argv[1];

  if(argc == 3) {
    G4int seed = atoi(argv[2]);
    simulation -> setSeed(seed);
  }
  
  simulation -> executeMacro(macrofile);
 
  simulation -> finish();
 
  delete simulation;

  G4cout << "End of job execution. " << G4endl;
  return 0;
}
