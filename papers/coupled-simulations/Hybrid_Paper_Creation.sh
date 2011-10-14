#!/bin/bash

### Name of the new paper directory
Paper="Temp"
### Name of repository home
Repository="https://svn.cct.lsu.edu/repos/saga-projects/papers/coupled-simulations"
### Name of reference Bibs 1: saga files
SAGA_Bib="https://svn.cct.lsu.edu/repos/saga-projects/papers/Compiled_bibentries"
### Name of reference Bibs 2: our Bibs
Local_Bib="https://svn.cct.lsu.edu/repos/saga-projects/papers/coupled-simulations/Bibs"

### Create the directory
svn mkdir $Repository/$Paper

### Checkout
svn co $Repository/$Paper

cd $Paper

### Getting Hybrid Bib file
svn co $Local_Bib

cd Bibs

### Reference to SAGA Bibs (get informed on ''export'')
svn export $SAGA_Bib/saga.bib 
svn export $SAGA_Bib/saga-related.bib

cd ..
