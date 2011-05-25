#ifndef PARTICLEBEAMSIMULATION_HH
#define PARTICLEBEAMSIMULATION_HH

#include "globals.hh"
#include "IG4Simulation.h"

class G4RunManager;


class ParticleBeamSimulation : virtual public DIANE::IG4Simulation {

 public:
   ParticleBeamSimulation(G4int);
   ~ParticleBeamSimulation();
  
   void setSeed(G4int seed);  
   G4bool initialize(int argc, char** argv);
   void executeMacro(std::string macroFileName);
   std::string getOutputFilename();
   void finish();

 private: 
   G4String outputFileName;
   G4int seed; 
   G4RunManager* runManager;
};

#endif
