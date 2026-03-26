#include <iostream>
#include "counters.hpp"

class LinkedList {
private:
    struct Node {
        int data;
        Node *next;

        Node(int v) : data(v), next(nullptr) {}
    };

    Node *head;
    Counters c{};

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
        c.inserts++;

        if (contains(value))
            return false;

        Node *n = new Node(value);

        c.structural_ops++;
        n->next = head;
        head = n;

        return true;
    }

    bool contains(int value) {
        c.lookups++;
        Node *curr = head;

        while (curr) {
            c.comparisons++;
            if (curr->data == value)
                return true;

            curr = curr->next;
        }

        return false;
    }

    bool erase(int value) {
        c.deletes++;
        Node *curr = head;
        Node *prev = nullptr;

        while (curr) {
            c.comparisons++;
            if (curr->data == value) {

                c.structural_ops++;
                if (prev)
                    prev->next = curr->next;
                else
                    head = curr->next;

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

     void save(string filename,bool dict=true){
        c.saveCounters(filename,dict);
    }

    Counters getCounters() {
        return c;
    }
};