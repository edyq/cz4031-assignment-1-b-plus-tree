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
    Node *parentPtr;            // Pointer to an array of keys in this node; check float/double
    vector<uint32_t> keys;
    int numKeys;                // Current number of keys in this node;
    int maxKeys;
    bool isLeaf;                // Whether this node is a leaf node;
    friend class BPTree;        // BPTree can access Node's private variables;

public:
    ~Node(){};
};

class InternalNode:Node {
private:
    vector<Node *> pointers;

public:
    InternalNode(int maxNumKeys, Node *parentPtr);  // Constructor

    vector<Node *>getChildNodes() {
        return pointers;
    }

    ~InternalNode(){};

    Node *getParentPtr() {
        return parentPtr;
    }
};

class LeafNode:Node {
private:
    vector<shared_ptr<Block>> blocks;              // Pointer to an array of blocks storing the records
    LeafNode *nextNode;             // Pointer to the neighboring leaf node

public:
    LeafNode(int maxNumKeys, Node *parentPtr);      // Constructor

    ~LeafNode(){};

    vector<shared_ptr<Block>> getBlocks() {
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

    void insertInternal(uint32_t key);

    void removeInternal(uint32_t key);

public:
    BPTree(int maxKeys);        // Constructor

    ~BPTree(){};

    void search(uint32_t lbKey, uint32_t ubKey);

    void insert(uint32_t key);

    int remove(uint32_t key);

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