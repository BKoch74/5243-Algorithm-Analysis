// ------------------------------
// Counters: what Python expects
// ------------------------------

#pragma once
#include <iostream>
#include "json.hpp"
#include <fstream>


using namespace std;
using json = nlohmann::json;

struct Counters {
    long long comparisons = 0;
    long long structural_ops = 0;
    long long inserts = 0;
    long long deletes = 0;
    long long lookups = 0;
    long long resize_events = 0;

    friend std::ostream& operator<<(std::ostream& os, const Counters &o){
        return os << "Comp:" << o.comparisons
                  << " Ops:" << o.structural_ops
                  << " Inserts: " << o.inserts << std::endl;
    }

    void saveCounters(const std::string& filename = "counters.json", bool asDict=true) const {
        json j;

        if(asDict){
            j["comparisons"] = comparisons;
            j["structural_ops"] = structural_ops;
            j["inserts"] = inserts;
            j["deletes"] = deletes;
            j["lookups"] = lookups;
            j["resize_events"] = resize_events;
        }else{
            j = {comparisons,structural_ops,inserts,deletes, lookups,resize_events};
        }

        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing");
        }

        file << j.dump(4); // pretty print with 4-space indent
    }
};