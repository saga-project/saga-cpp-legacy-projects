#!/bin/bash

for((f=0; f < 1000; f++))
do 
   for((c=0; c < 3000000; c++))
   do
      number=$[ ( $RANDOM % 4 )]
      if test $number -eq 0
      then
         echo -n 'a' >> file$f
      elif test $number -eq 1
      then
         echo -n 'c' >> file$f
      elif test $number -eq 2
      then
         echo -n 't' >> file$f
      else
         echo -n 'g' >> file$f
      fi
   done
done
