# Algoritmi
Gruppo 4

Compile command 
> g++ -O3 -o mmkp -I . *.cpp -lm

Run command 1
> timeout -s 2 60s ./mmkp -t 60 -i instance

Run command 2
> timeout -s 2 1s ./mmkp -t 1 -i instance


How to run everything step by step:
1. Compile main
2. Delete `std_stat_file.csv` and just to be sure the output folders too
3. Run `./run_std.sh`
4. Run `./verify_std.sh`
5. Check result and cry

![](https://media.tenor.com/P3RqQUUK9BAAAAAd/rip-juice-cry.gif)
