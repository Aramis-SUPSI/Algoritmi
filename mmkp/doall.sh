#!/bin/bash
cd MMKP_GR04;
rm -r out_standard;
cd ..;
rm std_stat_file.csv;

echo "Elimino file ...";
sleep 2;


cd MMKP_GR04;
g++ -O3 -o mmkp -I . *.cpp -lm;
cd ..;
sed -i -e 's/\r$//' run_std.sh
./run_std.sh;
sed -i -e 's/\r$//' verify_std.sh
./verify_std.sh;
