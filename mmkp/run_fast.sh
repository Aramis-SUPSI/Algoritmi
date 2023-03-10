#!/bin/bash

cwd=$(pwd);
# List all groups folders
for i in $(ls -d MMKP_GR*);
do
  echo "Running for group " ${i};
  cd ${i};

  # Create out_standard folder if it does not exist
  mkdir -p out_fast;

  for full_name in $(ls ../Instances/unknown/mmkp_*.txt);
  do
    instance=$(basename ${full_name})
    # Copy instance file
    cp ${full_name} ${instance}

    # Run algorithm with timeout and store user time
    /usr/bin/time -o ${instance}.time -f "%e" timeout -s 2 1s ./mmkp -i ${instance} -t 1

    # Backup instance, output and time files
    mv ${instance} out_fast;
    mv ${instance}.out out_fast;
    mv ${instance}.time out_fast;
  done
  cd $cwd;
done