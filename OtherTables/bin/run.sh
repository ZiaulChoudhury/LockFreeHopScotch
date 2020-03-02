#!/bin/bash
#for i in `seq 2 2 40`
for i in `seq 0 10 60`
do
   #echo "Threads = $i"
   echo "Update = $i"
	 #./clht_lf -i 1000000 -n $i -r 1048576 -u 20 
	 ./clht_lf -i 1000000 -n 12 -r 1048576 -u $i 
   echo "------------------------------------------------------"
done

