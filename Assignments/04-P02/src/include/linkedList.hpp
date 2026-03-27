#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "counters.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class LinkedList {
private:
    struct Node {
        int data;
        Node *next;
        Node(int v) : data(v), next(nullptr) {}
    };

    Node *head;
    Counters c{};

    // Private helper — traverses without touching lookups counter
    bool _contains(int value) {
        Node *curr = head;
        while (curr) {
            c.comparisons++;
            if (curr->data == value) return true;
            curr = curr->next;
        }
        return false;
    }

public:
    LinkedList() : head(nullptr) {}

    ~LinkedList() {
        Node *curr = head;
        while (curr) {
            Node *temp = curr;
            curr = curr->next;
            c.structural_ops++;
            delete temp;
        }
    }

    bool insert(int value) {
        c.inserts++;                // count every attempt
        if (_contains(value)) return false;  // use private helper, won't inflate lookups

        Node *n = new Node(value);
        c.structural_ops++;
        n->next = head;
        head = n;
        return true;
    }

    bool contains(int value) {
        c.lookups++;
        return _contains(value);    // delegate to private helper
    }

    bool erase(int value) {
        c.deletes++;                // count every attempt
        Node *curr = head;
        Node *prev = nullptr;
        while (curr) {
            c.comparisons++;
            if (curr->data == value) {
                c.structural_ops++;
                if (prev) prev->next = curr->next;
                else head = curr->next;
                delete curr;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    void print() const {
        Node *curr = head;
        while (curr) {
            std::cout << curr->data << " ";
            curr = curr->next;
        }
        std::cout << "\n";
    }

    void reset() {
        Node *curr = head;
        while (curr) {
            Node *temp = curr;
            curr = curr->next;
            delete temp;
        }
        head = nullptr;
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

    void save(string filename, bool dict = true) { c.saveCounters(filename, dict); }
    Counters getCounters() { return c; }
};