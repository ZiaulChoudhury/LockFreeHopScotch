#!/bin/bash
for i in `seq 2 2 40`
do
   echo "Threads = $i"
   ./Hopscotch $i
   echo "-------------------------------------------------------"
done

