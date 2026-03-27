#include "binaryHeap.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc,char**argv) {
    BinaryHeap binaryHeap;

    if(argc < 2){
        cout<<"Error: You need a filename!"<<endl;
        cout<<"Usage: ./dbst ../work_files/workload_A_1000.json"<<endl;
    }

    binaryHeap.runJobFile(argv[1]);

    cout<<binaryHeap.getCounters();

    string fname = string(argv[2]);

    binaryHeap.save(fname,true);
}