#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstring>
#include "data.h"
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <numeric>
#include <algorithm>


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

void signalHandler( int signum ) {
   std::cout << "Running finalizing code. Interrupt signal (" << signum << ") received.\n";


   exit(signum);  
}

int main(int argc, char *argv[]) {

    // register signal SIGINT and signal handler  
    signal(SIGINT, signalHandler); 

    char* timelimit = getOption(argc, argv, std::string("-t").c_str());
    char* input = getOption(argc, argv, std::string("-i").c_str());

    if (timelimit == nullptr || input == nullptr){
        std::cout << "Parameters are not correctly specified!\n";
        std::exit(1);
    }

    int inttimelimit = atoi(timelimit);
    std::cout << "Instance name:" << input << "\n" ;
    std::cout << "Time limit:" << inttimelimit << "\n" ;
    std::string output(input);
    output.append(".out");
    std::cout << "Output name:" << output << "\n";

    data instance;
    instance.read_input(input);
    

    /* ******************** */
    /* Write your code here */
    /* ******************** */

    std::vector<int> capacities_check(instance.nclasses);
    std::copy(instance.capacities.begin(), instance.capacities.end(), capacities_check.begin());
    std::vector<std::vector<int>> items(instance.nclasses, std::vector<int>(instance.nitems.size()));
    std::vector<std::vector<int>> weights_mean(instance.nclasses, std::vector<int>(instance.nitems.size()));
    for (int i = 0; i < instance.nclasses; i++) {
      for (int j = 0; j < instance.nitems[i]; j++) {
        float sum = 0.0f;
        for (auto z = 0; z < instance.nresources; z++) {
          sum += instance.weights[i][j * instance.nresources + z];
        }
        const float mean = sum / (float)instance.nresources;

        items[i][j] = j;
        weights_mean[i][j] = mean;
      }
      std::sort(items[i].begin(), items[i].end(), [&](int y, int t) {
          return (float)instance.values[i][y] / weights_mean[i][y] > (float)instance.values[i][t] / weights_mean[i][t];
      });
    }

    for (int i = 0; i < instance.nclasses; i++) {
      bool found = false;
      for (int j = 0; j < instance.nitems[i]; j++) {
        int item = items[i][j];
        bool acceptable = true;
        for (auto z = 0; z < instance.nresources; z++) {
          if(!(capacities_check[z] >= instance.weights[i][j * instance.nresources + z])){
            acceptable = false;
            break;
          }
        }
        if(acceptable){
          instance.solution[i] = item;
          
          for (auto z = 0; z < instance.nresources; z++) {
            capacities_check[z] -=  instance.weights[i][j * instance.nresources + z];
          }

          found = true;
          break;
        }
      }
      if(!found){
        instance.solution[i] = 0;
      }
    }



    /*    
    std::vector<std::vector<float>> items_score;
    items_score.resize(instance.nclasses);
    
    for (auto i = 0; i < instance.nclasses; i++) {
      items_score[i].resize(instance.nitems[i]);
      for (auto j = 0; j < instance.nitems[i]; j++) {
        float sum = 0.0f;
        for (auto z = 0; z < instance.nresources; z++) {
          sum += instance.weights[i][j * instance.nresources + z];
        }
        const float weigths_mean = sum / (float)instance.nresources;
        const float value_over_weigths_mean = (float)instance.values[i][j] / weigths_mean;
        items_score[i][j] = value_over_weigths_mean;
      }
    }*/
    

    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
            outfile << instance.solution[i] << " ";
    }
    outfile.close();
}
