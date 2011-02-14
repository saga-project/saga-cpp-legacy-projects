#!/usr/bin/env python

import sys
import saga
import os

# input comes from STDIN (standard input)
file1=sys.argv[1]
nbr_reduces=sys.argv[2]
f=open(file1,"r")

part_nbr=[]
sorted_part_nbr=[]
for i in range(0,int(nbr_reduces)):
    p_name="part-"+str(i) 
    print os.path.split(file1)[0]+"/"+ p_name
    part_nbr.append(open( file1 + p_name,'w'))
    
for line in f:
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into words
    words = line.split('\t')
    l=hash(words[0])%int(nbr_reduces)
    part_nbr[l].write( ",".join(words) +"\n" )

for i in range(0,int(nbr_reduces)):
    part_nbr[i].close()

for i in range(0,int(nbr_reduces)):
    p_name="part-"+str(i) 
    sorted_part_nbr.append(open( file1 + "sorted-"+ p_name,'w'))

for i in range(0,int(nbr_reduces)):
    p_name="part-"+str(i) 
    part_file=open( file1 + p_name,'r')     
    lines=[] # give lines variable a type of list
    for line in part_file: lines.append(line)
    lines.sort()
    for l in lines:
	sorted_part_nbr[i].write(l) 

for i in range(0,int(nbr_reduces)): 
    sorted_part_nbr[i].close()
    p_name="part-"+str(i)
    os.remove(file1+p_name)
