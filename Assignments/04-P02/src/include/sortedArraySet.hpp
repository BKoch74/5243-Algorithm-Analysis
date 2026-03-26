#pragma once

#include <cstddef>
#include <iostream>
#include "counters.hpp"

class SortedArraySet {
private:
    int *data;
    std::size_t count;
    std::size_t capacity;
    Counters c{};

    void resize(std::size_t newCapacity) {
        c.resize_events++;
        int *newData = new int[newCapacity];

        for (std::size_t i = 0; i < count; i++) {
            newData[i] = data[i];
            c.structural_ops++;
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

    // Returns the index where value is found,
    // or where it should be inserted to maintain sorted order.
    std::size_t lowerBound(int value) {
        std::size_t left = 0;
        std::size_t right = count;

        while (left < right) {
            std::size_t mid = left + (right - left) / 2;

            c.comparisons++;
            if (data[mid] < value) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }

        return left;
    }

public:
    explicit SortedArraySet(std::size_t initialCapacity = 8)
        : data(new int[initialCapacity]), count(0), capacity(initialCapacity) {}

    ~SortedArraySet() {
        delete[] data;
    }

    std::size_t size() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }

    bool contains(int value) {
        c.lookups++;
        if (count == 0) {
            return false;
        }

        std::size_t pos = lowerBound(value);

        c.comparisons++;
        return pos < count && data[pos] == value;
    }

    bool insert(int value) {
        c.inserts++;
        std::size_t pos = lowerBound(value);

        c.comparisons++;
        // Duplicate guard: do not insert if already present
        if (pos < count && data[pos] == value) {
            return false;
        }

        // Grow array if full
        if (count == capacity) {
            resize(capacity * 2);
        }

        // Shift elements right to make room
        for (std::size_t i = count; i > pos; i--) {
            data[i] = data[i - 1];
            c.structural_ops++;
        }

        c.structural_ops++;
        data[pos] = value;
        count++;

        return true;
    }

    bool erase(int value) {
        c.deletes++;
        if (count == 0) {
            return false;
        }

        std::size_t pos = lowerBound(value);

        c.comparisons++;
        if (pos >= count || data[pos] != value) {
            return false;
        }

        // Eager delete:
        // shift everything left immediately to close the gap
        for (std::size_t i = pos; i + 1 < count; i++) {
            data[i] = data[i + 1];
            c.structural_ops++;
        }

        c.structural_ops++;
        count--;
        return true;
    }

    void print() const {
        std::cout << "[";
        for (std::size_t i = 0; i < count; i++) {
            std::cout << data[i];
            if (i + 1 < count) {
                std::cout << ", ";
            }
        }
        std::cout << "]\n";
    }

    void reset() {
        delete[] data;
        data = new int[capacity];
        count = 0;
        c = {};
    }

    void runJobFile(std::string fname)
    {
        std::ifstream f(fname);
        json j = json::parse(f);

        for (auto &element : j)
        {
            string op = element["op"];
            int val = element["value"];

            if (op == "insert") insert(val);
            else if (op == "contains") contains(val);
            else if (op == "delete") erase(val);
        }
    }

    void save(string filename="sortedArraySet",bool dict=true){
        c.saveCounters(filename,dict);
    }

    Counters getCounters() {
        return c;
    }
};