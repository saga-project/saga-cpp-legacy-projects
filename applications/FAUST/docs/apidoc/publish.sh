#!/bin/bash

./makedoc.sh

scp -r html/* oweidner@macpro01.cct.lsu.edu:Sites/faust/

