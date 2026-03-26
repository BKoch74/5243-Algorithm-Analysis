#pragma once

#include "counters.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <string>

using json = nlohmann::json;

using namespace std;

class Bst
{
protected:
    struct Node
    {
        int data;
        Node *left;
        Node *right;
        int height;

        explicit Node(int value)
            : data(value), left(nullptr), right(nullptr), height(1) {}
    };

    Node *root;
    Counters c{};

    bool insert(Node *&node, int value)
    {
        c.comparisons++;
        if (!node)
        {
            c.structural_ops++;
            node = new Node(value);
            return true;
        }

        c.comparisons++;
        if (value < node->data)
        {
            return insert(node->left, value);
        }

        c.comparisons++;
        if (value > node->data)
        {
            return insert(node->right, value);
        }

        return false;
    }

    bool contains(Node *node, int value)
    {
        c.comparisons++;
        if (!node)
        {
            return false;
        }

        c.comparisons++;
        if (value == node->data)
        {
            return true;
        }

        c.comparisons++;
        if (value < node->data)
        {
            return contains(node->left, value);
        }

        return contains(node->right, value);
    }

    Node *findMin(Node *node)
    {
        while (node)
        {
            c.comparisons++;
            if (!node->left)
            {
                break;
            }
            node = node->left;
        }
        return node;
    }

    bool erase(Node *&node, int value)
    {
        c.comparisons++;
        if (!node)
        {
            return false;
        }

        c.comparisons++;
        if (value < node->data)
        {
            return erase(node->left, value);
        }

        c.comparisons++;
        if (value > node->data)
        {
            return erase(node->right, value);
        }

        if (!node->left && !node->right)
        {
            c.structural_ops++;
            delete node;
            node = nullptr;
            return true;
        }

        if (!node->left)
        {
            c.structural_ops++;
            Node *temp = node;
            node = node->right;
            delete temp;
            return true;
        }

        if (!node->right)
        {
            c.structural_ops++;
            Node *temp = node;
            node = node->left;
            delete temp;
            return true;
        }

        Node *successor = findMin(node->right);
        c.structural_ops++;
        node->data = successor->data;
        return erase(node->right, successor->data);
    }

    void clear(Node *node)
    {
        c.comparisons++;
        if (!node)
        {
            return;
        }

        clear(node->left);
        clear(node->right);
        c.structural_ops++;
        delete node;
    }

public:
    Bst() : root(nullptr)
    {
    }

    void reset(){
        clear(root);
        root = nullptr;
        c = {}; 
    }

    void save(string filename,bool dict=true){
        c.saveCounters(filename,dict);
    }

    virtual ~Bst()
    {
        clear(root);
    }

    Counters getCounters(){
        return c;
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

    bool insert(int value)
    {
        c.inserts++;
        return insert(root, value);
    }

    bool contains(int value)
    {
        c.lookups++;
        return contains(root, value);
    }

    bool erase(int value)
    {
        c.deletes++;
        return erase(root, value);
    }

    virtual const char *name() const
    {
        return "BST";
    }
};