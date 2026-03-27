#pragma once

#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include "counters.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class SortedArraySet {
private:
    int *data;
    std::size_t count;
    std::size_t capacity;
    std::size_t initialCapacity;  // stored so reset() can restore original size
    Counters c{};

    void resize(std::size_t newCapacity) {
        c.resize_events++;
        int *newData = new int[newCapacity];
        for (std::size_t i = 0; i < count; i++) {
            newData[i] = data[i];
            c.structural_ops++;     // each element copy is a structural op
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

    std::size_t lowerBound(int value) {
        std::size_t left = 0;
        std::size_t right = count;
        while (left < right) {
            std::size_t mid = left + (right - left) / 2;
            c.comparisons++;
            if (data[mid] < value) left = mid + 1;
            else right = mid;
        }
        return left;
    }

public:
    explicit SortedArraySet(std::size_t initialCapacity = 10)
        : data(new int[initialCapacity]),
          count(0),
          capacity(initialCapacity),
          initialCapacity(initialCapacity) {}

    ~SortedArraySet() { delete[] data; }

    std::size_t size() const { return count; }
    bool empty() const { return count == 0; }

    bool contains(int value) {
        c.lookups++;
        if (count == 0) return false;

        std::size_t pos = lowerBound(value);

        c.comparisons++;            // final check: is element at pos equal to value?
        return pos < count && data[pos] == value;
    }

    bool insert(int value) {
        c.inserts++;                // count every attempt
        std::size_t pos = lowerBound(value);

        c.comparisons++;            // final check: is element at pos a duplicate?
        if (pos < count && data[pos] == value) return false;

        if (count == capacity) resize(capacity * 2);

        // shift elements right to make room
        for (std::size_t i = count; i > pos; i--) {
            data[i] = data[i - 1];
            c.structural_ops++;     // each shift is a structural op
        }

        data[pos] = value;          // place the new element
        c.structural_ops++;
        count++;
        return true;
    }

    bool erase(int value) {
        c.deletes++;                // count every attempt
        std::size_t pos = lowerBound(value);

        c.comparisons++;            // final check: is element at pos the one we want?
        if (pos >= count || data[pos] != value) return false;

        // shift elements left to fill the gap
        for (std::size_t i = pos; i + 1 < count; i++) {
            data[i] = data[i + 1];
            c.structural_ops++;     // each shift is a structural op
        }
        count--;
        return true;
    }

    void print() const {
        std::cout << "[";
        for (std::size_t i = 0; i < count; i++) {
            std::cout << data[i];
            if (i + 1 < count) std::cout << ", ";
        }
        std::cout << "]\n";
    }

    void reset() {
        delete[] data;
        data = new int[initialCapacity];  // restore to original capacity
        count = 0;
        capacity = initialCapacity;
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

    void save(string filename = "sortedArraySet", bool dict = true) {
        c.saveCounters(filename, dict);
    }

    Counters getCounters() { return c; }
};