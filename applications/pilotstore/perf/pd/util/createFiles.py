#!/bin/env python
import os
import sys


DIRNAME="/work/luckow/scenarios"

os.system("rm -rf " + DIRNAME)
os.system("mkdir " + DIRNAME)

for i in range(0, 16):
    number_files = 2**i
    scenario_dir = DIRNAME+ "/"+str(number_files)
    os.system("mkdir " + scenario_dir) 
    for j in range(0, number_files):
        file_object = open(scenario_dir+"/"+str(j), 'w')
        file_object.write("") 
        file_object.close( )

