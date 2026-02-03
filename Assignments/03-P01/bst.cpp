#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Node {
    int data;
    Node *left;
    Node *right;

    Node(int x) {
        data = x;
        left = right = nullptr;
    }
};

class GraphvizBST {
public:
    static void saveDotFile(const std::string &filename, const std::string &dotContent) {
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << dotContent;
            outFile.close();
            std::cout << "DOT file saved: " << filename << std::endl;
        } else {
            std::cerr << "Error: Could not open file " << filename << std::endl;
        }
    }

    static std::string generateDot(const Node *root) {
        std::string dot = "digraph BST {\n";
        dot += "    node [fontname=\"Arial\"];\n";
        dot += generateDotHelper(root);
        dot += "}\n";
        return dot;
    }

private:
    static std::string generateDotHelper(const Node *node) {
        if (!node)
            return "";
        std::string result;
        if (node->left) {
            result += "    " + std::to_string(node->data) + " -> " + std::to_string(node->left->data) + " [label=\"L\"];\n";
            result += generateDotHelper(node->left);
        } else {
            std::string nullNode = "nullL" + std::to_string(node->data);
            result += "    " + nullNode + " [shape=point];\n";
            result += "    " + std::to_string(node->data) + " -> " + nullNode + ";\n";
        }
        if (node->right) {
            result += "    " + std::to_string(node->data) + " -> " + std::to_string(node->right->data) + " [label=\"R\"];\n";
            result += generateDotHelper(node->right);
        } else {
            std::string nullNode = "nullR" + std::to_string(node->data);
            result += "    " + nullNode + " [shape=point];\n";
            result += "    " + std::to_string(node->data) + " -> " + nullNode + ";\n";
        }
        return result;
    }
};

class Bst {
    Node *root;

    void _print(Node *subroot) {
        if (!subroot) {
            return;
        } else {
            _print(subroot->left);
            cout << subroot->data << " ";
            _print(subroot->right);
        }
    }
    void _insert(Node *&subroot, int x) {
        if (!subroot) { // if(root == nullptr)
            subroot = new Node(x);
        } else {
            if (x < subroot->data) {
                _insert(subroot->left, x);
            } else {
                _insert(subroot->right, x);
            }
        }
    }
    int _ipl(Node *root, int depth = 0) {
        if (!root)
            return 0; // Base case: Empty subtree contributes 0 to IPL
        return depth + _ipl(root->left, depth + 1) + _ipl(root->right, depth + 1);
    }

public:
    Bst() { root = nullptr; }
    void insert(int x) { _insert(root, x); }
    bool search(int key) { return 0; }
    void print() { _print(root); }
    void saveDotFile(const std::string &filename) {
        std::string dotContent = GraphvizBST::generateDot(root);
        GraphvizBST::saveDotFile(filename, dotContent);
    }

    /**
     * Computes the Internal Path Length (IPL) of a Binary Search Tree (BST).
     *
     * Definition:
     * The Internal Path Length (IPL) of a BST is the sum of the depths of all nodes in the tree.
     * The depth of a node is the number of edges from the root to that node.
     *
     * Example:
     *        10
     *       /  \
     *      5    15
     *     / \     \
     *    2   7    20
     *
     * IPL = (depth of 10) + (depth of 5) + (depth of 15) + (depth of 2) + (depth of 7) + (depth of 20)
     *     = 0 + 1 + 1 + 2 + 2 + 2 = 8
     *
     * @param root Pointer to the root node of the BST.
     * @param depth Current depth of the node (default is 0 for the root call).
     * @return The sum of depths of all nodes (Internal Path Length).
     */
    int ipl() {
        return _ipl(root);
    }

    void bst_delete(int val) {
        Node* parent = nullptr;
        Node* curr = root;

        while (curr && curr->data != val) {
            parent = curr;
            if (val < curr->data)
                curr = curr->left;
            else
                curr = curr->right;
        }
        if (!curr) return;

        if (curr->left && curr->right) {
            Node* succParent = curr;
            Node* succ = curr->right;

            while (succ->left) {
                succParent = succ;
                succ = succ->left;
            }
            curr->data = succ->data;
            parent = succParent;
            curr = succ;
        }
        Node* child;

        if (curr->left != nullptr) {
            child = curr->left;
        }
        else {
            child = curr->right;
        }

        if (!parent) {
            root = child;
        }
        else if (parent->left == curr) {
            parent->left = child;
        }
        else {
            parent->right = child;
        }

        delete curr;
    }
};

bool unique_value(int *arr, int n, int x) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == x) {
            return false;
        }
    }
    return true;
}

int main() {
    {
        Bst tree;
        tree.insert(10);

        tree.saveDotFile("t1_before.dot");
        tree.bst_delete(10);
        tree.saveDotFile("t1_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(5);

        tree.saveDotFile("t2_before.dot");
        tree.bst_delete(10);
        tree.saveDotFile("t2_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(15);

        tree.saveDotFile("t3_before.dot");
        tree.bst_delete(10);
        tree.saveDotFile("t3_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(5);
        tree.insert(15);
        tree.insert(2); 

        tree.saveDotFile("t4_before.dot");
        tree.bst_delete(2);
        tree.saveDotFile("t4_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(5);
        tree.insert(15);
        tree.insert(20); 

        tree.saveDotFile("t5_before.dot");
        tree.bst_delete(20);
        tree.saveDotFile("t5_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(5);
        tree.insert(2);  

        tree.saveDotFile("t6_before.dot");
        tree.bst_delete(5);
        tree.saveDotFile("t6_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(15);
        tree.insert(20); 

        tree.saveDotFile("t7_before.dot");
        tree.bst_delete(15);
        tree.saveDotFile("t7_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(5);
        tree.insert(15);
        tree.insert(2);
        tree.insert(7);

        tree.saveDotFile("t8_before.dot");
        tree.bst_delete(5);
        tree.saveDotFile("t8_after.dot");
    }
    {
        Bst tree;
        tree.insert(10);
        tree.insert(5);
        tree.insert(15);
        tree.insert(2);
        tree.insert(7);
        tree.insert(12);
        tree.insert(20);

        tree.saveDotFile("t9_before.dot");
        tree.bst_delete(10);
        tree.saveDotFile("t9_after.dot");
    }
    {
        Bst tree;
        tree.insert(1);
        tree.insert(2);
        tree.insert(3);
        tree.insert(4);
        tree.insert(5);

        tree.saveDotFile("t10_before.dot");
        tree.bst_delete(3);
        tree.saveDotFile("t10_after.dot");
    }
    
    /*
    * Original Code:
    * 
    Bst tree;
    int root = pow(2, 15) / 2;
    int max = pow(2, 15) - 1;
    vector<int> arr;
    arr.push_back(root);
    tree.insert(root);
    for (int i = 1; i < 5000; i++) {
        int r = rand() % max;
        while (!unique_value(arr.data(), arr.size(), r)) {
            r = rand() % max;
        }
        tree.insert(r);
        arr.push_back(r);
    }

    tree.print();
    tree.saveDotFile("bst_snapshot.dot");

    Bst tree2;
    tree2.insert(10);
    tree2.insert(5);
    tree2.insert(15);
    tree2.insert(2);
    tree2.insert(7);
    tree2.insert(20);
    cout << "Internal Path Length: " << tree2.ipl() << endl;
    */
}
