#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
using namespace std;

InternalNode::InternalNode(int maxKeys) {
    keys = new float[maxKeys];
    numKeys = 0;
    isLeaf = false;
    pointers = new Node[maxKeys + 1];
}

LeafNode::LeafNode(int maxKeys) {
    keys = new float[maxKeys];
    numKeys = 0;
    isLeaf = true;
    vector<Block> blocks;
    nextNode = nullptr;
}
