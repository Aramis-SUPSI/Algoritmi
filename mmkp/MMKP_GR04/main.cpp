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

    std::vector<int> capacities_check(instance.nresources);
    std::copy(instance.capacities.begin(), instance.capacities.end(), capacities_check.begin());
    std::vector<int> classes(instance.nclasses);

    for(int i = 0; i < instance.nclasses; i++) {
      classes[i] = i;
    }

    //Start
    // until all classes are used, keep choosing the best item


    //find classses with only one item, and put it in the solution
    for (int i = 0; i < classes.size(); i++) {
        if(instance.nitems[classes[i]] == 1){
            instance.solution[classes[i]] = 0;



            //and update resources
            for (auto z = 0; z < instance.nresources; z++) {
                capacities_check[z] -= instance.weights[classes[i]][z];
            }
            classes.erase(classes.begin() + i);

            i--;
        }
    }

    while(classes.size() > 0) {

      //keep track of the best item found so far
      int best_class_found = 0;
      int best_item_found = 0;

      float value_of_best = 1000000.0f;


      for (int i = 0; i < classes.size(); i++) {
        for (int j = 0; j < instance.nitems[classes[i]]; j++) {
          float sum = 0.0f;
          for (auto z = 0; z < instance.nresources; z++) {
              // if the item is too heavy, skip it
            if(capacities_check[z] - instance.weights[classes[i]][j * instance.nresources + z] < 0){
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
              value = sum;
            //value = (float)instance.values[classes[i]][j] / sum - sum;
          } else{
            value = value_of_best+1;
          }
          
          if(value < value_of_best){
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

    //Simulated Annealing
    int temperature = 100;
    double alpha = 0.9f;


    std::vector<std::vector<int>> neighborhood(neighbors, std::vector<int>(instance.solution.size()));
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


    //std::cout << "capacities: " << "\n";
    //check if solution is feasible
    is_feasible = true;
    for (int i = 0; i < instance.nresources; i++) {
        //std::cout << capacities_check[i] << ' ';
        if (capacities_check[i] < 0) {

            is_feasible = false;
        }
    }
    std::cout << std::endl;


    /*float max_occup = 0.0f;
    for (int i = 0; i < instance.nresources; i++) {
        float occup = (float)instance.capacities[i] - (float)capacities_check[i];
        occup /= (float)instance.capacities[i];
        if(occup > max_occup){
            max_occup = occup;
        }
    }*/

    //std::cout << "feasible: " << is_feasible << "\n";

    int cnt = 700;
    while (cnt>0) {
        //drop temperature after 10 iterations
        if (cnt >= 400 && cnt % 50 == 0) {
            temperature *= alpha;
        }

        for (int i = 0; i < neighbors; i++) {

            int newMaxValue = maxValue;
            neighborhood[i] = instance.solution;
            //std::copy(instance.solution.begin(), instance.solution.end(), neighborhood[i].begin());

            //local copy of capacities
            std::vector<int> capacities_check_local(capacities_check);
            //std::copy(capacities_check.begin(), capacities_check.end(), capacities_check_local.begin());

            int rndClass;

            //pick a random class that has more than one item
            rndClass = std::rand() % instance.solution.size();
            while(instance.nitems[rndClass] == 1){
                rndClass = std::rand() % instance.solution.size();
            }


            int rndItem;
            //if the class has two items, swap them
            if (instance.nitems[rndClass] == 2) {
                neighborhood[i][rndClass] = (instance.solution[rndClass] + 1) % 2;
                rndItem = neighborhood[i][rndClass];
                /*int tmp = neighborhood[i][rndClass];
                neighborhood[i][rndClass] = (neighborhood[i][rndClass] + 1) % 2;
                neighborhood[i][(rndClass + 1) % instance.solution.size()] = tmp;*/

            }else {
                //if the class has more than two items, swap a random item with another random item
                while ((rndItem = std::rand() % instance.nitems[rndClass]) == instance.solution[rndClass]);
                neighborhood[i][rndClass] = rndItem;
            }

            bool acceptable = true;
            //for each resource, add the weight of the current item from capacities_check_local
            for (int j = 0; j < instance.nresources; j++) {
                capacities_check_local[j] += instance.weights[rndClass][instance.solution[rndClass] * instance.nresources + j];
                capacities_check_local[j] -= instance.weights[rndClass][rndItem * instance.nresources + j];


                if (capacities_check_local[j] < 0) {
                    acceptable = false;
                }
            }

            //calculate new max value
            newMaxValue -= instance.values[rndClass][instance.solution[rndClass]];
            newMaxValue += instance.values[rndClass][rndItem];

            //check remaining weight of this solution
            int remainingWeight_local = 0;
            for (int i = 0; i < instance.nresources; i++) {
                remainingWeight_local += capacities_check_local[i];
            }

            //check highest occupied weight % of this solution
            /*float max_occup_local = 0.0f;
            for (int i = 0; i < instance.nresources; i++) {
                float occup = (float)instance.capacities[i] - (float)capacities_check_local[i];
                occup /= (float)instance.capacities[i];
                if(occup > max_occup_local){
                    max_occup_local = occup;
                }
            }*/

            neighborhoodValues[i][0] = acceptable;
            neighborhoodValues[i][1] = newMaxValue;
            neighborhoodValues[i][2] = remainingWeight_local;
            neighborhoodWeights[i] = capacities_check_local;

            //std::copy(capacities_check_local.begin(), capacities_check_local.end(), neighborhoodWeights[i].begin());
            //print instance.solution
            //std::cout << "current solution: " << std::endl;
            //for (int i = 0; i < instance.solution.size(); i++) {
                //std::cout << instance.solution[i] << ' ';
            //}

            //print neighborhood[i]
            //std::cout << "\nneighborhood[" << i << "]: " << std::endl;
            //for (int j = 0; j < neighborhood[i].size(); j++) {
                //std::cout << neighborhood[i][j] << ' ';
            //}
            //std::cout << std::endl;
        }

        for (int i = 0; i < neighbors; i++) {

            //if the neighbor is better, accept it
            if (
                    ((is_feasible && (neighborhoodValues[i][0] && neighborhoodValues[i][1] > maxValue)) ||
                            //(!is_feasible && !neighborhoodValues[i][0] && neighborhoodValues[i][2] == remainingWeight && neighborhoodValues[i][1] >= maxValue) ||
                            (!is_feasible && (neighborhoodValues[i][0] || neighborhoodValues[i][2] >= remainingWeight)))
                    ) {

                if(is_feasible && neighborhoodValues[i][1] > maxValue){
                    //std::cout << "improvement: " << neighborhoodValues[i][1] << " " << maxValue << std::endl;
                }

                if (is_feasible && !neighborhoodValues[i][0]) {
                    //std::cout << "ERROR! Feasible into not feasible " << std::endl;
                }

                //std::copy(neighborhood[i].begin(), neighborhood[i].end(), instance.solution.begin());
                //std::copy(neighborhoodWeights[i].begin(), neighborhoodWeights[i].end(), capacities_check.begin());
                instance.solution = neighborhood[i];
                capacities_check = neighborhoodWeights[i];

                remainingWeight = neighborhoodValues[i][2];
                maxValue = neighborhoodValues[i][1];
                is_feasible = neighborhoodValues[i][0];
            }
            //chance to accept worse solution via simulated annealing, only if temperature > 0 and the new solution is feasible
            else if (cnt >= 400 && neighborhoodValues[i][0] && temperature > 0 && std::rand() % 100 < 100 * std::exp((maxValue - neighborhoodValues[i][1]) / temperature)) {
                //std::cout << "worse solution accepted" << std::endl;
                //std::copy(neighborhood[i].begin(), neighborhood[i].end(), instance.solution.begin());
                //std::copy(neighborhoodWeights[i].begin(), neighborhoodWeights[i].end(), capacities_check.begin());
                instance.solution = neighborhood[i];
                capacities_check = neighborhoodWeights[i];

                remainingWeight = neighborhoodValues[i][2];
                maxValue = neighborhoodValues[i][1];
                is_feasible = neighborhoodValues[i][0];

                break;
            }
        }



        cnt --;
    }

    std::cout << "capacities: " << "\n";
    //check if solution is feasible
    is_feasible = true;
    for (int i = 0; i < instance.nresources; i++) {
        std::cout << capacities_check[i] << ' ';
        if (capacities_check[i] < 0) {

            is_feasible = false;
        }
    }
    std::cout << std::endl;

    //std::cout << "feasible: " << is_feasible << "\n";

    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
            outfile << instance.solution[i] << " ";
    }
    outfile.close();
}
