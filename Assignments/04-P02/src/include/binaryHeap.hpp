#pragma once

#include <stdexcept>
#include <utility>
#include <vector>
#include <fstream>
#include <string>
#include "counters.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class BinaryHeap {
private:
    std::vector<int> data;
    Counters c{};

    int parent(int i) const { return (i - 1) / 2; }
    int left(int i) const { return 2 * i + 1; }
    int right(int i) const { return 2 * i + 2; }

    void siftUp(int i) {
        while (i > 0) {
            int p = parent(i);
            c.comparisons++;
            if (data[i] >= data[p]) break;
            c.structural_ops++;
            std::swap(data[i], data[p]);
            i = p;
        }
    }

    void siftDown(int i) {
        int n = static_cast<int>(data.size());
        while (true) {
            int l = left(i);
            int r = right(i);
            int smallest = i;

            if (l < n) {
                c.comparisons++;
                if (data[l] < data[smallest]) smallest = l;
            }

            if (r < n) {
                c.comparisons++;
                if (data[r] < data[smallest]) smallest = r;
            }

            if (smallest == i) break;

            c.structural_ops++;
            std::swap(data[i], data[smallest]);
            i = smallest;
        }
    }

    // Private linear scan for contains — does not touch lookups counter
    bool _contains(int value) {
        for (int i = 0; i < static_cast<int>(data.size()); i++) {
            c.comparisons++;
            if (data[i] == value) return true;
        }
        return false;
    }

public:
    BinaryHeap() = default;
    explicit BinaryHeap(const std::vector<int> &values) : data(values) { buildHeap(); }

    bool empty() const { return data.empty(); }
    std::size_t size() const { return data.size(); }

    // top() is a peek, not a lookup — no counter increment
    int top() {
        if (data.empty()) throw std::runtime_error("heap is empty");
        return data[0];
    }

    // contains() is the actual lookup operation
    bool contains(int value) {
        c.lookups++;
        return _contains(value);
    }

    void push(int value) {
        c.inserts++;
        data.push_back(value);
        siftUp(static_cast<int>(data.size()) - 1);
    }

    bool pop() {
        c.deletes++;          // count every attempt
        if (data.empty()) return false;

        if (data.size() == 1) {
            data.pop_back();
            c.structural_ops++;
            return true;
        }

        data[0] = data.back();
        data.pop_back();
        c.structural_ops++;
        siftDown(0);
        return true;
    }

    void buildHeap() {
        for (int i = static_cast<int>(data.size()) / 2 - 1; i >= 0; --i)
            siftDown(i);
    }

    void reset() {
        data.clear();
        c = {};
    }

    void runJobFile(std::string fname) {
        std::ifstream f(fname);
        json j = json::parse(f);
        for (auto &element : j) {
            string op = element["op"];
            int val = element["value"];
            if (op == "insert") push(val);
            else if (op == "delete") pop();
            else if (op == "contains") contains(val);  // now correctly calls contains
        }
    }

    void save(string filename, bool dict = true) { c.saveCounters(filename, dict); }
    Counters getCounters() { return c; }
};