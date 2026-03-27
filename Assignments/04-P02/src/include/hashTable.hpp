#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <fstream>
#include "counters.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class HashTable {
private:
    std::vector<std::vector<int>> table;
    std::size_t capacity;
    std::size_t count;          // track number of elements for load factor
    Counters c{};

    // Prime capacities from assignment spec
    const std::vector<std::size_t> PRIME_CAPACITIES{
        7, 11, 23, 47, 97, 197, 397, 797, 1597,
        3203, 6421, 12853, 25717, 51437
    };

    // Integer hash function
    std::size_t hash(int key) const {
        return static_cast<std::size_t>(key) * 2654435761u;
    }

    // Compress hash value into a valid bucket index
    std::size_t indexFor(int key) const {
        return hash(key) % capacity;
    }

    // Find the next prime capacity larger than the current one
    std::size_t nextPrime() const {
        for (std::size_t p : PRIME_CAPACITIES) {
            if (p > capacity) return p;
        }
        // If we exceed all primes, just double (fallback)
        return capacity * 2;
    }

    // Load factor: number of elements / table size
    double loadFactor() const {
        return static_cast<double>(count) / static_cast<double>(capacity);
    }

    // Rehash all elements into a new larger table
    void resize() {
        c.resize_events++;
        std::size_t newCapacity = nextPrime();
        std::vector<std::vector<int>> newTable(newCapacity);

        for (auto &bucket : table) {
            for (int key : bucket) {
                std::size_t idx = static_cast<std::size_t>(key) * 2654435761u % newCapacity;
                newTable[idx].push_back(key);
                c.structural_ops++;     // each rehash move is a structural op
            }
        }

        table = std::move(newTable);
        capacity = newCapacity;
    }

public:
    // Assignment spec: start with size 7
    explicit HashTable(std::size_t cap = 7)
        : table(cap), capacity(cap), count(0) {}

    bool insert(int key) {
        c.inserts++;                    // count every attempt

        std::size_t idx = indexFor(key);
        auto &bucket = table[idx];

        // Check for duplicate — each comparison counts
        for (int value : bucket) {
            c.comparisons++;
            if (value == key) return false;
        }

        bucket.push_back(key);
        c.structural_ops++;             // placing element in bucket
        count++;

        // Resize if load factor exceeds 0.75 (chained implementation)
        if (loadFactor() > 0.75) {
            resize();
        }

        return true;
    }

    bool contains(int key) {
        c.lookups++;                    // count every attempt

        std::size_t idx = indexFor(key);
        const auto &bucket = table[idx];

        for (int value : bucket) {
            c.comparisons++;
            if (value == key) return true;
        }

        return false;
    }

    bool erase(int key) {
        c.deletes++;                    // count every attempt

        std::size_t idx = indexFor(key);
        auto &bucket = table[idx];

        for (std::size_t i = 0; i < bucket.size(); i++) {
            c.comparisons++;
            if (bucket[i] == key) {
                // swap-pop delete: fast, order not preserved
                bucket[i] = bucket.back();
                bucket.pop_back();
                c.structural_ops++;     // the swap-pop is a structural op
                count--;
                return true;
            }
        }

        return false;
    }

    void reset() {
        table.clear();
        table.resize(7);                // reset back to starting prime
        capacity = 7;
        count = 0;
        c = {};
    }

    void runJobFile(std::string fname) {
        std::ifstream f(fname);
        json j = json::parse(f);
        for (auto &element : j) {
            string op = element["op"];
            int val = element["value"];
            if (op == "insert")        insert(val);
            else if (op == "delete")   erase(val);
            else if (op == "contains") contains(val);
        }
    }

    void save(string filename = "hashTable", bool dict = true) {
        c.saveCounters(filename, dict);
    }

    Counters getCounters() { return c; }
};