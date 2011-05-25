/** the module name should correspond to the $SIM_MODULE defined in GNUmakefile */
%module ParticleBeamSimulation

%{
#include "ParticleBeamSimulation.hh"
%}

%include "include/G4SWIGTypes.i"

%include "include/ParticleBeamSimulation.hh"
