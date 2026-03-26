#include "linkedList.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc,char**argv) {
    LinkedList linkedList;

    if(argc < 2){
        cout<<"Error: You need a filename!"<<endl;
        cout<<"Usage: ./dbst ../work_files/workload_A_1000.json"<<endl;
    }

    linkedList.runJobFile(argv[1]);

    cout<<linkedList.getCounters();

    linkedList.save("firstrunLinkedList",true);






}