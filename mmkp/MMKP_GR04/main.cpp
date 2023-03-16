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
#include <map>


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

    std::cout << argv << "\n";
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
    std::vector<int> classes(instance.nclasses);

    for(int i = 0; i < instance.nclasses; i++) {
      classes[i] = i;
    }

    while(classes.size() > 0) {
      int best_class_found = 0;
      int best_item_found = 0;
      float value_of_best = 0.0f;
      for (int i = 0; i < classes.size(); i++) {
        for (int j = 0; j < instance.nitems[classes[i]]; j++) {
          float sum = 0.0f;
          for (auto z = 0; z < instance.nresources; z++) {
            if((float)capacities_check[z] - (float)instance.weights[classes[i]][j * instance.nresources + z] <= 0){
              sum = 0.0f;
              break;
            } else{
              sum += (float)instance.weights[classes[i]][j * instance.nresources + z] / (float)capacities_check[z] * 100.0f;
            }
          }
          float value;
          if(sum != 0.0f){
            value = (float)instance.values[classes[i]][j] / sum;
          } else{
            value = 0.0f;
          }
          
          if(value > value_of_best){
            best_class_found = i;
            best_item_found = j;
            value_of_best = value;
          }
        }
      }

      instance.solution[classes[best_class_found]] = best_item_found;
      for (auto z = 0; z < instance.nresources; z++) {
        capacities_check[z] -= instance.weights[classes[best_class_found]][best_item_found * instance.nresources + z];
      }

      classes.erase(classes.begin() + best_class_found);
      
    }

    //End

    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
            outfile << instance.solution[i] << " ";
    }
    outfile.close();
}
