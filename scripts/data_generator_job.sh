#!/bin/bash
#PBS -l select=1:ncpus=1:mem=64gb 

# set max execution time
#PBS -l walltime=5:00:00
#set the queue  
#PBS -q short_cpuQ    

EXECUTABLE_PATH=./Parallel-Count-Min-sketch/data/data_generator.py
NUMBER_OF_IPS=1020848630
OUTPUT_PATH=./Parallel-Count-Min-sketch/data/data.bin
FILE_TYPE=bin
RUN_FOR_SECONDS=1800. # run for 30 minutes, ignoring the data size
module load python-3.10.14

python3 $EXECUTABLE_PATH $NUMBER_OF_IPS $OUTPUT_PATH $FILE_TYPE
