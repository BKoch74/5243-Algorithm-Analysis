#include "sortedArraySet.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc,char**argv) {
    SortedArraySet sortedArraySet;

    if(argc < 2){
        cout<<"Error: You need a filename!"<<endl;
        cout<<"Usage: ./dbst ../work_files/workload_A_1000.json"<<endl;
    }

    sortedArraySet.runJobFile(argv[1]);

    cout<<sortedArraySet.getCounters();

    string fname = string(argv[2]);

    sortedArraySet.save(fname,true);
}