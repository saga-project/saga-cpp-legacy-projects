#!/bin/bash

doxygen

scp -r html/* oweidner@macpro01.cct.lsu.edu:Sites/faust/

