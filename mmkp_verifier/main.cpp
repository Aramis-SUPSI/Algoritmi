#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstring>
#include "data.h"

char* getOption(int argc, char* argv[], const char* option)
{
  for( int i = 0; i < argc; ++i)
    if(strcmp(argv[i], option) == 0 )
      return argv[i+1];
  return nullptr;
}

inline bool exists (const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}

int main(int argc, char *argv[]) {

    char* timelimit = getOption(argc, argv, std::string("-t").c_str());
    char* input = getOption(argc, argv, std::string("-i").c_str());
    char* ingroup = getOption(argc, argv, std::string("-g").c_str());
    char* stats = getOption(argc, argv, std::string("-s").c_str());

    if (timelimit == nullptr || input == nullptr){
        std::cout << "Parameters are not correctly specified!\n";
        exit(1);
    }

    int inttimelimit = atoi(timelimit);
    std::cout << "Instance name:" << input << "\n" ;
    std::cout << "Time limit:" << inttimelimit << "\n" ;
    std::string output(input);
    output.append(".out");
    std::cout << "Output name:" << output << "\n";

    if ((ingroup == nullptr) || (stats == nullptr)){
        //Run trivial code to get a solution
        data instance;
        instance.read_input(input);
        std::ofstream outfile;
        outfile.open(output, std::ios_base::out);
        for (auto i = 0; i<instance.nclasses; i++){
                outfile << instance.solution[i] << " ";
        }
        outfile.close();
    }
    else {
        std::string time(input);
        time.append(".time");
        std::cout << "Time name:" << time << "\n";
        std::string group(ingroup);
        std::cout << "Group name:" << group << "\n";

        // Open stats in append
        std::ofstream outfile;
        outfile.open(stats, std::ios_base::app);

        if (!exists(input)) {
            outfile << group << "," << input << ",0.0,0.0,Input does not exist" << std::endl;
            outfile.close();
            exit(0);
        }

        if (!exists(output)) {
            outfile << group << "," << input << ",0.0,0.0,Output does not exist" << std::endl;
            outfile.close();
            exit(0);
        }

        if (!exists(time)) {
            outfile << group << "," << input << ",0.0,0.0,Time does not exist" << std::endl;
            outfile.close();
            exit(0);
        }

        data instance;
        int res = instance.read_input(input);
        res = instance.read_output(output);
        if (res == 1) {
            outfile << group << "," << input << ",0.0,0.0,Cannot open output" << std::endl;
            outfile.close();
            exit(0);
        }
        if (res == 2) {
            outfile << group << "," << input << ",0.0,0.0,Not enough selected items" << std::endl;
            outfile.close();
            exit(0);
        }
        if (res == 3) {
            outfile << group << "," << input << ",0.0,0.0,Selected item does not exist" << std::endl;
            outfile.close();
            exit(0);
        }
        res = instance.read_time(time);
        if (res == 1) {
            outfile << group << "," << input << ",0.0,0.0,Unable to read time" << std::endl;
            outfile.close();
            exit(0);
        }

        double value = 0.0;
        res = instance.verify_solution(&value);
        if (res == 1) {
            outfile << group << "," << input << ",0.0,0.0,Solution is not feasible" << std::endl;
            outfile.close();
            exit(0);
        }

        outfile << group << "," << input << "," << value << "," << instance.ptime << ",Solution is feasible"
                << std::endl;
        outfile.close();
        exit(0);
    }
}
