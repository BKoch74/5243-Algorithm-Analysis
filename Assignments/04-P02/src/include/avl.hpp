#pragma once

#include "bst.hpp"
#include <algorithm>

class Avl : public Bst {
private:
    int height(Node *node) const {
        return node ? node->height : 0;
    }

    int balanceFactor(Node *node) const {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void updateHeight(Node *node) {
        if (node) {
            node->height = 1 + std::max(height(node->left), height(node->right));
        }
    }

    Node *rotateRight(Node *y) {
        Node *x = y->left;
        Node *t2 = x->right;

        x->right = y;
        y->left = t2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    Node *rotateLeft(Node *x) {
        Node *y = x->right;
        Node *t2 = y->left;

        y->left = x;
        x->right = t2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node *rebalance(Node *node) {
        updateHeight(node);

        int bf = balanceFactor(node);

        // Left heavy
        if (bf > 1) {
            if (balanceFactor(node->left) < 0) {
                node->left = rotateLeft(node->left); // LR case
            }
            return rotateRight(node); // LL case
        }

        // Right heavy
        if (bf < -1) {
            if (balanceFactor(node->right) > 0) {
                node->right = rotateRight(node->right); // RL case
            }
            return rotateLeft(node); // RR case
        }

        return node;
    }

    Node *insert(Node *node, int value, bool &inserted) {
        if (!node) {
            inserted = true;
            return new Node(value);
        }

        if (value < node->data) {
            node->left = insert(node->left, value, inserted);
        } else if (value > node->data) {
            node->right = insert(node->right, value, inserted);
        } else {
            inserted = false; // duplicate
            return node;
        }

        return rebalance(node);
    }

    Node *erase(Node *node, int value, bool &erased) {
        if (!node) {
            erased = false;
            return nullptr;
        }

        if (value < node->data) {
            node->left = erase(node->left, value, erased);
        } else if (value > node->data) {
            node->right = erase(node->right, value, erased);
        } else {
            erased = true;

            // 0 or 1 child
            if (!node->left || !node->right) {
                Node *child = node->left ? node->left : node->right;
                delete node;
                return child;
            }

            // 2 children
            Node *successor = findMin(node->right);
            node->data = successor->data;
            node->right = erase(node->right, successor->data, erased);
        }

        return rebalance(node);
    }

public:
    Avl() : Bst() {}

    bool insert(int value) {
        bool inserted = false;
        root = insert(root, value, inserted);
        return inserted;
    }

    bool erase(int value) {
        bool erased = false;
        root = erase(root, value, erased);
        return erased;
    }

    const char *name() const {
        return "AVL";
    }

    int height() const {
        return height(root);
    }
};