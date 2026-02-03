## P01 - BST Delete
### Bryce Koch
### Description:

Program is used to build Binary Search Trees and test a newly implemented delete method. 
This delete method is designed to three main scenarios. Deleting a leaf node (node with no children),
deleting a node with one child, and deleting a node with two children.

### Files

|   #   | File             | Description                                        |
| :---: | ---------------- | -------------------------------------------------- |
|   1   | bst.cpp         | File that contains the main program around binary search trees.     |
|   2   | Test Cases  | Folder containing example outputs of different BST delete scenarios         |

### Explanation
In my testing, I considered ten distinct cases to ensure thorough coverage of possible BST deletion scenarios. 
These include: a single-node tree, a root with one left child, a root with one right child, leaf deletion 
in the left subtree, leaf deletion in the right subtree, one-child deletion in a left-heavy tree, one-child deletion 
in a right-heavy tree, two-children deletion for a non-root node, two-children deletion for the root node, and a skewed 
tree representing a degenerate case. By designing these varied test cases, I was able to verify that my deletion
implementation handles different tree shapes, sizes, and complexities correctly.

For nodes with two children, I implemented Inorder Predecessor as the primary method for choosing a replacement value. 
I also included support for Inorder Successor in case there is no largest value in the left subtree or if the tree
requires an alternative replacement strategy. I chose the Inorder Predecessor specifically because, given a lower limit on non-negative
values in my dataset, it reduces the risk of creating infinite branches in a right-heavy tree and simplifies pointer management in edge cases.

During implementation, I faced several challenges, particularly with null pointer handling. Traversing subtrees safely while maintaining
parent and child links required careful checks to avoid segmentation faults. Additionally, ensuring that leaf nodes, single-child nodes, 
and root nodes were all correctly reconnected after deletion required careful consideration of different pointer configurations. Overall,
these test cases and the Inorder Predecessor strategy provided a robust framework for validating the correctness of BST deletions across multiple scenarios.
