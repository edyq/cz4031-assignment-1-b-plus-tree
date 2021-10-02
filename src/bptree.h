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
private:
    Node *parentPtr;            // Pointer to an array of keys in this node; check float/double
    vector<uint32_t> keys;
    int numKeys;                // Current number of keys in this node;
    int maxKeys;

    bool isLeaf;                // Whether this node is a leaf node;
    friend class BPTree;        // BPTree can access Node's private variables;
    vector<Node *> pointers;      // attr for non-leaf node
    vector<vector<shared_ptr<Block>>> blocks;              // attr for leaf node
    Node *nextNode;         // attr for leaf node
    Node *preNode;         // attr for leaf node

public:
    Node(int maxNumKeys, Node *parentPtr, bool isLeaf);

    bool isLeafNode(){
        return isLeaf;
    }
    vector<Node *>getChildNodes() {
        return pointers;
    }
    vector<vector<shared_ptr<Block>>> getBlocks() {
        return blocks;
    }

    vector<uint32_t> getKeys(){
        return keys;
    }

    Node *getNextNode() {
        return nextNode;
    }
    Node *getParentPtr() {
        return parentPtr;
    }


    ~Node(){};
};

struct SearchResult {
    vector<Node *> accessedNodes;
    vector<shared_ptr<Block>> accessedBlocks;
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
    BPTree(int maxNumKeys);        // Constructor

    ~BPTree(){};

    vector<SearchResult> search(uint32_t lbKey, uint32_t ubKey);

    SearchResult searchKey(uint32_t key);

    void insert(uint32_t key, shared_ptr<Block> blockAddress);

    void remove(uint32_t key);

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