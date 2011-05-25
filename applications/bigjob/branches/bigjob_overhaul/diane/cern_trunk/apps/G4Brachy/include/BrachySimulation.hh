#include "globals.hh"

#include "Randomize.hh"
#include "IG4Simulation.h"
#include "G4RunManager.hh"
class BrachySimulation : virtual public DIANE::IG4Simulation 
{
public:
  BrachySimulation(G4int);
  ~BrachySimulation();
  
  void setSeed(G4int seed);  
  G4bool initialize(int argc, char** argv);
  void executeMacro(std::string macroFileName);
  std::string getOutputFilename();
  void finish();

private: 
  G4int seed; 
  G4RunManager* pRunManager;
};

