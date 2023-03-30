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
    std::vector<std::vector<int>> items(instance.nclasses);
    std::vector<std::vector<float>> sums(instance.nclasses);

    float total_sum = 0.0f;
    int sum_count = 0;
    for(int i = 0; i < instance.nclasses; i++) {
      classes[i] = i;
      items[i].resize(instance.nitems[i]);
      sums[i].resize(instance.nitems[i]);
      for(int j = 0; j < instance.nitems[i]; j++){
        items[i][j] = j;
        float sum = 0.0f;
        for (int z = 0; z < instance.nresources; z++) {
            sum += (float)instance.weights[i][j * instance.nresources + z] / (float)capacities_check[z] * 100.0f;
        }
        sums[i][j] = (float)instance.values[i][j] / sum - sum;
        total_sum += sums[i][j];
        sum_count++;
      }
    }
    
    for (int i = 0; i < instance.nclasses; i++) {
      int j = 0; // Initialize index variable outside the loop
      while (j < items[i].size()) { // Iterate over valid elements of the vector
        float avg_sum = total_sum / sum_count;
        float diff = sums[i][j] - avg_sum;
        float threshold;
        if (diff >= 0) {
          threshold = sums[i][j] * 0.30; // 30% threshold
        } else {
          threshold = sums[i][j] * 0.15; // 15% threshold
        }
        if (abs(diff) > threshold) { //Not in threshold
          if (items[i].size() > 1) { //and not the last remaining item
            items[i].erase(items[i].begin() + j); //Erase
          } else {
            break;
          }
        } else {
          j++; // Increment index variable only if no element was erased
        }
      }
    } 

    //Sort classes based on sums[class].size()
    auto compareClasses = [&](const int& class1, const int& class2) {
        return items[class1].size() < items[class2].size();
    };
    std::sort(classes.begin(), classes.end(), compareClasses);

    for(int i = 0; i < classes.size(); i++){
      int best_item = 0;
      float best_item_value = 0;
      for(int j = 0; j < items[classes[i]].size(); j++){
        bool is_feasible = true;
        for (auto z = 0; z < instance.nresources; z++) {
          if((float)capacities_check[z] - (float)instance.weights[classes[i]][items[classes[i]][j] * instance.nresources + z] <= 0){
            is_feasible = false;
          }
        }
        if(is_feasible){
          if(sums[classes[i]][items[classes[i]][j]] > best_item_value){
            best_item = items[classes[i]][j];
            best_item_value = sums[classes[i]][items[classes[i]][j]];
          }
        }
      }
      instance.solution[classes[i]] = best_item;
      for (auto z = 0; z < instance.nresources; z++) {
        capacities_check[z] -= instance.weights[classes[i]][best_item * instance.nresources + z];
      }
    }
    
/*
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
          float sum_best = 0.0f; 
          for (auto z = 0; z < instance.nresources; z++) {
              // if the item is too heavy, skip it
            if((float)capacities_check[z] - (float)instance.weights[classes[i]][j * instance.nresources + z] <= 0){
              sum = 0.0f;
              sum_best = 0.0f;
              break;
            } else{
              //peso_occupato / peso rimanente + peso_tot_occupato / peso_tot
              sum += (float)instance.weights[classes[i]][j * instance.nresources + z] / (float)capacities_check[z] * 100.0f * pow(((float)instance.capacities[z] - (float)capacities_check[z]) / (float)instance.capacities[z], 2.0f);
              sum_best += (float)instance.weights[classes[i]][j * instance.nresources + z] / (float)capacities_check[z] * 100.0f * pow(((float)instance.capacities[z] - (float)capacities_check[z]) / (float)instance.capacities[z], 2.0f);
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
      
    }*/

    //End

    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
            outfile << instance.solution[i] << " ";
    }
    outfile.close();
}
