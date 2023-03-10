#!/bin/bash

cwd=$(pwd);
for i in $(ls -d MMKP_GR*);
do
  cd ${i}/out_fast;
  echo "*********************";
  echo "Verifying for group " ${i};
  echo "*********************";
  echo "                     ";
  for full_name in $(ls ./mmkp_*.txt);
  do
    ../../mmkp_verify -i ${full_name} -t 60 -s ../../fast_stat_file.csv -g ${i}
  done
  cd $cwd;
done