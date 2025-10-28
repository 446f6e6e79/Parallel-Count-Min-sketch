#!/bin/bash
#PBS -l select=1:ncpus=1:mem=2gb 

# set max execution time
#PBS -l walltime=0:01:00
#set the queue  
#PBS -q short_cpuQ    

module load python-3.10.14

python3 ./Parallel-Count-Min-sketch/data/data_generator.py 52 Parallel-Count-Min-sketch/data/data.bin bin 
