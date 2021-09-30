#ifndef BP_TREE_H
#define BP_TREE_H

#define ORDER_V 5
#define MAXNUM_KEY (ORDER_V * 2)
#define MAXNUM_POINTER (MAXNUM_KEY + 1)
#define MAXNUM_DATA (ORDER_V * 2)

#include "block.h"
#include <vector>
using namespace std;
typedef int KEY_TYPE;

class Node {
protected:
    float *keys;                // Pointer to an array of keys in this node;
    int numKeys;                // Current number of keys in this node;
    bool isLeaf;                // Whether this node is a leaf node;
    friend class BPTree;        // BPTree can access Node's private variables;
};

class InternalNode:Node {
private:
    Node *pointers;             // Pointers to an array of Node;

public:
    InternalNode(int maxKeys);  // Constructor

    Node *getNodes() {
        return pointers;
    }
};

class LeafNode:Node {
private:
    vector<Block> blocks;              // Pointer to an array of blocks storing the records
    LeafNode *nextNode;             // Pointer to the neighboring leaf node

public:
    LeafNode(int maxKeys);      // Constructor

    vector<Block> getBlocks() {
        return blocks;
    }

    LeafNode *getNextNode() {
        return nextNode;
    }
};

class BPTree {
private:
    Node *root;                 // Pointer to the root node of the tree
    int maxKeys;                // Max number of keys in a node in the BP tree
    int levels;                 // Height of the BP tree
    int numNodes;               // Total number of nodes in the BP tree

    void insertInternal(float key);

    void removeInternal(float key);

public:
    BPTree(int maxKeys);        // Constructor

    void search(float lbKey, float ubKey);

    void insert(float key);

    int remove(float key);

    Node *getFirstChildNode() const;

    [[nodiscard]] int getLevels() const {
        return levels;
    }

    [[nodiscard]] int getNumNodes() const {
        return numNodes;
    }

    [[nodiscard]] int getMaxKeys() const {
        return maxKeys;
    }

    [[nodiscard]] Node *getRoot() const {
        return root;
    }
};


#endif