#!/bin/bash
#PBS -l select=1:ncpus=5:mem=2gb 

# set max execution time
#PBS -l walltime=0:01:00
#set the queue  
#PBS -q short_cpuQ   
module load mpich-3.2
mpirun.actual -n 5 ./Parallel-Count-Min-sketch/bin/CMsketch ./Parallel-Count-Min-sketch/data/data.bin ./Parallel-Count-Min-sketch/data/process_info.csv 0.01 0.99 -l pack:excl