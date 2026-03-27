#include "hashTable.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
    HashTable hashTable;

    if (argc < 2) {
        cout << "Error: You need a workload file and an output filename!" << endl;
        cout << "Usage: ./dhashtable ../work_files/workload_A_1000.json results.json" << endl;
    }

    hashTable.runJobFile(argv[1]);

    cout << hashTable.getCounters();

    string fname = string(argv[2]);

    hashTable.save(fname, true);

    return 0;
}