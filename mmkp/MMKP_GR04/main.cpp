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
#include <math.h>
#include <vector>
#include <random>
#include <chrono>


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

    //Start
    // until all classes are used, keep choosing the best item
    while(classes.size() > 0) {
      //keep track of the best item found so far
      int best_class_found = 0;
      int best_item_found = 0;

      float value_of_best = 0.0f;


      for (int i = 0; i < classes.size(); i++) {
        for (int j = 0; j < instance.nitems[classes[i]]; j++) {
          float sum = 0.0f;
          for (auto z = 0; z < instance.nresources; z++) {
              // if the item is too heavy, skip it
            if((float)capacities_check[z] - (float)instance.weights[classes[i]][j * instance.nresources + z] <= 0){
              sum = 0.0f;
              break;
            } else{
              //peso_occupato / peso rimanente + peso_tot_occupato / peso_tot
              float cap_inv = 1.0f / (float)instance.capacities[z];
              float cap_check = (float)capacities_check[z];
              float cap_diff = cap_check - (float)instance.capacities[z];
              float cap_diff_sq = cap_diff * cap_diff;

              sum += (float)instance.weights[classes[i]][j * instance.nresources + z] / (float)capacities_check[z] * 100.0f * cap_diff_sq * cap_inv * cap_inv;
            }
          }


          float value;
          if(sum != 0.0f){
            value = (float)instance.values[classes[i]][j] / sum - sum;
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
    int maxValue;
    int neighbors = 10;
    int remainingWeight;
    bool is_feasible;

    std::vector<std::vector<int>> neighborhood(neighbors, std::vector<int>(instance.nitems.size()));
    std::vector<std::vector<int>> neighborhoodWeights(neighbors, std::vector<int>(instance.nresources));
    std::vector<std::vector<int>> neighborhoodValues(neighbors, std::vector<int>(3));

    std::srand(std::time(nullptr));

    //calculate remaining weight
    for (int i = 0; i < instance.nresources; i++) {
        remainingWeight += capacities_check[i];
    }

    for (int i = 0; i < instance.solution.size(); i++) {
        maxValue += instance.values[i][instance.solution[i]];
    }

    //check if solution is feasible
    is_feasible = true;
    for (int i = 0; i < instance.nresources; i++) {
        if (capacities_check[i] < 0) {
            is_feasible = false;
        }
    }

    float max_occup = 0.0f;
    for (int i = 0; i < instance.nresources; i++) {
        float occup = (float)instance.capacities[i] - (float)capacities_check[i];
        occup /= (float)instance.capacities[i];
        if(occup > max_occup){
            max_occup = occup;
        }
    }

    int cnt = 50;
    while (cnt>0) {
        for (int i = 0; i < neighbors; i++) {
            int newMaxValue = maxValue;
            std::copy(instance.solution.begin(), instance.solution.end(), neighborhood[i].begin());

            //local copy of capacities
            std::vector<int> capacities_check_local(instance.nclasses);
            std::copy(capacities_check.begin(), capacities_check.end(), capacities_check_local.begin());

            int rndClass = std::rand() % instance.nclasses;
            int rndItem = std::rand()%instance.nitems[rndClass];
            neighborhood[i][rndClass] = rndItem;

            bool acceptable = true;
            for (auto z = 0; z < instance.nresources; z++) {
                capacities_check_local[z] += instance.weights[rndClass][instance.solution[rndClass] * instance.nresources + z];
                if (capacities_check_local[z] < instance.weights[rndClass][rndItem * instance.nresources + z]) {
                    acceptable = false;
                }

                capacities_check_local[z] -= instance.weights[rndClass][rndItem * instance.nresources + z];
            }

            newMaxValue -= instance.values[rndClass][instance.solution[rndClass]];
            newMaxValue += instance.values[rndClass][rndItem];

            //check remaining weight of this solution
            int remainingWeight_local = 0;
            for (int i = 0; i < instance.nresources; i++) {
                remainingWeight_local += capacities_check_local[i];
            }

            //check highest occupied weight % of this solution
            float max_occup_local = 0.0f;
            for (int i = 0; i < instance.nresources; i++) {
                float occup = (float)instance.capacities[i] - (float)capacities_check_local[i];
                occup /= (float)instance.capacities[i];
                if(occup > max_occup_local){
                    max_occup_local = occup;
                }
            }

            neighborhoodValues[i][0] = acceptable;
            neighborhoodValues[i][1] = newMaxValue;
            neighborhoodValues[i][2] = remainingWeight_local;
            neighborhoodWeights[i] = capacities_check_local;
        }

        for (int i = 0; i < neighbors; i++) {
            if (
                    ((is_feasible && (neighborhoodValues[i][0] && neighborhoodValues[i][1] > maxValue))
                            || (!is_feasible && !neighborhoodValues[i][0] && neighborhoodValues[i][2] == remainingWeight && neighborhoodValues[i][1] >= maxValue) ||
                            (!is_feasible && (neighborhoodValues[i][0] || neighborhoodValues[i][2] > remainingWeight)))
                    ) {

                std::copy(neighborhood[i].begin(), neighborhood[i].end(), instance.solution.begin());
                std::copy(neighborhoodWeights[i].begin(), neighborhoodWeights[i].end(), capacities_check.begin());
                remainingWeight = neighborhoodValues[i][2];
                maxValue = neighborhoodValues[i][1];
                is_feasible = neighborhoodValues[i][0];
            }
        }

        cnt --;
    }

    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
            outfile << instance.solution[i] << " ";
    }
    outfile.close();
}
