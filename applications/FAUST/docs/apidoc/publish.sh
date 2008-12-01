#!/bin/bash

doxygen

scp -r cpp/* oweidner@macpro01.cct.lsu.edu:Sites/faust/

