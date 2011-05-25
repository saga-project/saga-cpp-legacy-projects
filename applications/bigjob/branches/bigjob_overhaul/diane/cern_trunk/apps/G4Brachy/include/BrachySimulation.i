/** the module name should correspond to the $SIM_MODULE defined in GNUmakefile */
%module BrachySimulation

%{
#include "BrachySimulation.hh"
%}

%include "include/G4SWIGTypes.i"

%include "include/BrachySimulation.hh"
