
#include "ParticleBeamSimulation.hh"
#include <cstdlib>


int main(int argc, char** argv){

  if(argc < 2 || argc > 3) {
      G4cerr << "ERROR Wrong number of arguments" 
             << G4endl;
      return -1;
  }

  G4String macrofile;

  ParticleBeamSimulation* simulation = new ParticleBeamSimulation(0);  

  simulation -> initialize(argc, argv);

  macrofile = argv[1];

  if(argc == 3) {
    G4int seed = atoi(argv[2]);
    simulation -> setSeed(seed);
  }
  
  simulation -> executeMacro(macrofile);
 
  simulation -> finish();
 
  delete simulation;

  G4cout << "INFORMATION End of job execution. " 
         << G4endl;

  return 0;
}
