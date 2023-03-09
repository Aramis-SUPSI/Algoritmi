#include "data.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>

//#define MYDEBUG

using namespace std;

int data::read_input(const string& instance) {
    cout << "Read instance: " << instance << " ";

    fstream newfile;
    newfile.open(instance, ios::in); // apre file
    if (newfile.is_open()) { // se è aperto
        string line;
        getline(newfile, line); // leggi linea
        istringstream sizes(line); // size = 100 | 10
        sizes >> nclasses >> nresources; // attribuisce i valori 100 e 10 alle variabili

        nitems.resize(nclasses); // attribuizione dimensione vettore numero di elementi
        values.resize(nclasses); // attribuizione dimensione vettore dei valori
        weights.resize(nclasses); // attribuizione dimensione vettore dei pesi
        solution.resize(nclasses); // attribuizione dimensione vettore delle soluzioni
        capacities.resize(nresources); // attribuizione dimensione vettore delle capacità

        getline(newfile, line); // leggi prossima linea
        istringstream resources(line); // line = x | x | x | ...  dove x è il valore della capacità letta
        for (auto i = 0; i < nresources; i++) {
            resources >> capacities[i]; // inserisce valori letti nell'array delle capacità
        }

        for (auto i = 0; i < nclasses; i++) {
            solution[i] = 0; // imposta valore 0 come default
            getline(newfile, line); // chiene nuova linea
            istringstream isnitems(line); // line = x | x | x | ... dove x è il numero degli elementi contenuti nella classe [?] todo: check da file
            isnitems >> nitems[i]; // inserisce valori letti nell'array del numero di elementi
            values[i].resize(nitems[i]); // modifica la dimensione dell'array
            weights[i].resize(nitems[i] * nresources);  // modicia dimensione dei pesi in modo da passare da una marice tridimensionale a un vettore
            for (auto j = 0; j < nitems[i]; j++) {
                getline(newfile, line); // legge prossima linea
                istringstream itemdata(line); // line = x | x | x | ... dove x è il peso di ogni elemento
                itemdata >> values[i][j]; // inserisce primo valore letto nell'array dei valori
                for (auto z = 0; z < nresources; z++) {
                    itemdata >> weights[i][j * nresources + z]; // inserisce gli altri valori letti nell array dei pesi
                }
            }
        }
        newfile.close(); // chiusura file
        cout << "Done." << endl << endl; // stampa a schermo

#ifdef MYDEBUG
        // Print for debug purposes
        cout << nclasses << " " << nresources << endl;
        for (auto i = 0; i<nresources; i++){
                cout << capacities[i] << " ";
        }
        cout << endl;
        for (auto i = 0; i<nclasses; i++){
                cout << nitems[i] << endl;
                for (auto j = 0; j<nitems[i]; j++){
                    cout << values[i][j] << " ";
                    for (auto z = 0; z<nresources; z++){
                        cout << weights[i][j * nresources + z] << " ";
                    }
                    cout << endl;
                }
        }
#endif

    }
    return 0;
}

int data::read_output(const string& instance) {
    cout << "Read solution: " << instance << " ";

    fstream newfile;
    newfile.open(instance, ios::in);

    if (newfile.is_open()) {
        string line;
        getline(newfile, line);
        istringstream items(line);

        solution.resize(nclasses);
        for (auto i = 0; i < nclasses; i++) {
            if (!(items >> solution[i])) {
                // Solution does not contain enough integer values
                newfile.close();
                return 2;
            }
            if (solution[i] >= nitems[i]) {
                // Solution contains an item that does not exist
                newfile.close();
                return 3;
            }
        }
        newfile.close();
    } else {
        // Cannot open file
        return 1;
    }

    cout << "Done." << endl;
    return 0;
}

int data::read_time(const string& instance) {
    fstream newfile;
    newfile.open(instance, ios::in);
    if (newfile.is_open()) {
        newfile >> ptime;
    } else {
        return 1;
    }
    return 0;
}

int data::verify_solution(double *val) {
    cout << "Verify solution: ";
    double tmpval = 0.0;
    vector<int> consumption(nresources, 0);
    for (auto i = 0; i < nclasses; i++) {
        tmpval += values[i][solution[i]];
        for (auto k = 0; k < nresources; k++) {
            consumption[k] += weights[i][solution[i] * nresources + k];
            if (consumption[k] > capacities[k]) {
                cout << "Unfeasible on resource " << k << endl;
                return 1;
            }
        }
    }
    *val = tmpval;
    cout << "Feasible, value " << tmpval << endl;
    return 0;
}
