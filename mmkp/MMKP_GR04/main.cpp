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
#include <list>
#include <ctime>


char *getOption(int argc, char *argv[], const char *option) {
    for (int i = 0; i < argc; ++i)
        if (strcmp(argv[i], option) == 0)
            return argv[i + 1];
    return nullptr;
}

inline bool exists(const std::string &name) {
    return (access(name.c_str(), F_OK) != -1);
}

void signalHandler(int signum) {
    std::cout << "Running finalizing code. Interrupt signal (" << signum << ") received.\n";


    exit(signum);
}

int main(int argc, char *argv[]) {

    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    std::cout << argv << "\n";
    char *timelimit = getOption(argc, argv, std::string("-t").c_str());
    char *input = getOption(argc, argv, std::string("-i").c_str());

    if (timelimit == nullptr || input == nullptr) {
        std::cout << "Parameters are not correctly specified!\n";
        std::exit(1);
    }

    int inttimelimit = atoi(timelimit);
    std::cout << "Instance name:" << input << "\n";
    std::cout << "Time limit:" << inttimelimit << "\n";
    std::string output(input);
    output.append(".out");
    std::cout << "Output name:" << output << "\n";

    data instance;
    instance.read_input(input);


    /* ******************** */
    /* Write your code here */
    /* ******************** */


    // Viene dichiarato un vettore di interi "capacities_check" con dimensione "instance.nclasses".
    std::vector<int> capacities_check(instance.nclasses);
    // Viene copiato il contenuto del vettore "instance.capacities" nel vettore "capacities_check".
    std::copy(instance.capacities.begin(), instance.capacities.end(), capacities_check.begin());
    // Vengono dichiarate due matrici di interi "items" e "weights_mean" con dimensioni "instance.nclasses" x "instance.nitems.size()".
    std::vector<std::vector<int>> items(instance.nclasses, std::vector<int>(instance.nitems.size()));
    std::vector<std::vector<int>> weights_mean(instance.nclasses, std::vector<int>(instance.nitems.size()));

    // indice (posizione) classi in cui non è stato preso nulla
    std::list<int> emptyClassesIndex;

    // Viene eseguito un ciclo for sulla variabile "i" da 0 a "instance.nclasses".
    for (int i = 0; i < instance.nclasses; i++) {
        // Viene eseguito un ciclo for sulla variabile "j" da 0 a "instance.nitems[i]".
        for (int j = 0; j < instance.nitems[i]; j++) {
            // Viene dichiarata una variabile float "sum" e inizializzata a 0.
            float sum = 0.0f;
            // Viene eseguito un ciclo for sulla variabile "z" da 0 a "instance.nresources".
            for (auto z = 0; z < instance.nresources; z++) {
                // Viene aggiunto alla variabile "sum" il valore "instance.weights[i][j * instance.nresources + z]".
                sum += instance.weights[i][j * instance.nresources + z];
            }
            // Viene calcolata la media dei pesi dell'oggetto su tutte le dimensioni e assegnata alla variabile "mean".
            const float mean = sum / (float) instance.nresources;

            // Viene assegnato alla matrice "items" l'indice dell'oggetto "j" nella classe "i".
            items[i][j] = j;
            // Viene assegnato alla matrice "weights_mean" il valore della media dei pesi dell'oggetto "j" nella classe "i".
            weights_mean[i][j] = mean;
        }

        // Viene eseguito un ordinamento della matrice "items" in base al rapporto valore/peso medio degli oggetti, utilizzando una funzione lambda.
        std::sort(items[i].begin(), items[i].end(), [&](int y, int t) {
            return (float) instance.values[i][y] / weights_mean[i][y] >
                   (float) instance.values[i][t] / weights_mean[i][t];
        });
    }

    // Viene eseguito un ciclo for sulla variabile "i" da 0 a "instance.nclasses".
    for (int i = 0; i < instance.nclasses; i++) {
        // Viene dichiarata una variabile booleana "found" e inizializzata a false.
        bool found = false;
        // Viene eseguito un ciclo for sulla variabile "j" da 0 a "instance.nitems[i]".
        for (int j = 0; j < instance.nitems[i]; j++) {
            // Viene assegnato alla variabile "item" l'indice dell'oggetto "j" nella classe "i".
            int item = items[i][j];
            // Viene dichiarata una variabile booleana "acceptable" e inizializzata a true.
            bool acceptable = true;

            // Viene eseguito un ciclo for sulla variabile "z" da 0 a "instance.nresources".
            for (auto z = 0; z < instance.nresources; z++) {
                // Viene verificato se la capacità disponibile nella dimensione "z" è inferiore al peso dell'oggetto "j" nella classe "i".
                if (capacities_check[z] < instance.weights[i][j * instance.nresources + z]) {
                    // Se la capacità disponibile è inferiore al peso dell'oggetto, la variabile "acceptable" viene impostata a false e il ciclo for viene interrotto.
                    acceptable = false;
                    break;
                }
            }
            // Se l'oggetto "j" è accettabile, viene assegnato alla soluzione della classe "i" l'indice dell'oggetto "j".
            if (acceptable) {
                // Viene eseguito un ciclo for sulla variabile "z" da 0 a "instance.nresources".
                instance.solution[i] = item;
                // Viene sottratto dalla capacità disponibile nella dimensione "z" il peso dell'oggetto "j" nella classe "i".
                for (auto z = 0; z < instance.nresources; z++) {
                    capacities_check[z] -= instance.weights[i][j * instance.nresources + z];
                }
                // La variabile "found" viene impostata a true.
                found = true;
                break;
            } else {
                emptyClassesIndex.push_back(i);
            }
        }
        // Se nessun oggetto è stato trovato per la classe "i", viene assegnato alla soluzione il valore 0.
        if (!found) {
            instance.solution[i] = 0;
        }
    }


    int neighbors = 10;
    std::vector<std::vector<int>> neighborhood(neighbors, std::vector<int>(instance.nitems.size()));
    std::srand(std::time(nullptr));
    int maxValue;
    int newMaxValue;
    std::vector<std::vector<int>> neighborhoodValues(neighbors, std::vector<int>(2));

    int cnt = 10;
    while (cnt>0) {

        for (int i = 0; i < instance.solution.size(); i++) {
            maxValue += instance.values[i][instance.solution[i]];
        }

        for (int i = 0; i < neighbors; i++) {
            newMaxValue = maxValue;
            std::copy(instance.solution.begin(), instance.solution.end(), neighborhood[i].begin());

            int rndClass = std::rand() % instance.nclasses;
            int rndItem = std::rand()%instance.nitems[rndClass];
            neighborhood[i][rndClass] = rndItem;

            bool acceptable = true;
            for (auto z = 0; z < instance.nresources; z++) {
                capacities_check[z] += instance.weights[rndClass][instance.solution[rndClass] * instance.nresources + z];
                if (capacities_check[z] < instance.weights[rndClass][rndItem * instance.nresources + z]) {
                    acceptable = false;
                    break;
                }
            }

            if (acceptable) {
                newMaxValue -= instance.values[rndClass][instance.solution[rndClass]];
                newMaxValue += instance.values[rndClass][rndItem];
            }

            neighborhoodValues[i][0] = acceptable;
            neighborhoodValues[i][1] = newMaxValue;
        }

        for (int i = 0; i < neighbors; i++) {
            if (neighborhoodValues[i][0] && neighborhoodValues[i][1] > maxValue) {
                std::copy(neighborhood[i].begin(), neighborhood[i].end(), instance.solution.begin());
                maxValue = newMaxValue;
            }
        }

        cnt --;
    }

    //End

    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i < instance.nclasses; i++) {
        outfile << instance.solution[i] << " ";
    }

    outfile.close();

}