#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
using namespace std;

Node::Node(int maxNumKeys, Node *parentPtr, bool isLeaf) {
    parentPtr = parentPtr;
    keys = * new vector<uint32_t>;
    numKeys = 0;
    maxKeys = maxNumKeys;
    isLeaf = isLeaf;
    pointers = * new vector<Node *>;
    blocks = * new vector<shared_ptr<Block>>;
    nextNode = nullptr;
}

BPTree::BPTree(int maxNumKeys) {
    root = nullptr;
    maxKeys = maxNumKeys;
    levels = 0;
    numNodes = 0;
}
