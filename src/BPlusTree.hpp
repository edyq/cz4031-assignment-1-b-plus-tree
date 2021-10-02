#pragma once

#include <string>
#include <vector>
#include <queue>
#include "Node.hpp"
#include "block.h"
//#include "Record.hpp"
#include "LeafNode.hpp"
#include "InternalNode.hpp"
//#include "RecordPointer.hpp"
using namespace std;
class BPlusTree
{
public:
    BPlusTree(int numKeys);
    void insert(uint32_t key, vector<shared_ptr<Block>> b);
    void remove(uint32_t key);
    vector<shared_ptr<Block>> BPlusTree::searchRecord(uint32_t startKey, uint32_t endKey)

    // Printer methods usde to print cpmplete or part of bpt
    void printTree();
    void printLeaves();
    // Helper function for printers
    void printEmptyTree();
    void printNoneEmptyTree();
    void printCurrentRank(std::queue<Node *> *argCurrentRank, std::queue<Node *> *argNextRank);

    // Below are helper functions

    // find the leaf node that stores the given key
    LeafNode *findLeafNode(uint32_t key);
    // insert a node into a given parent node
    void insertInternal(InternalNode *parentNode, float key, Node *childNode);
    bool isEmptyTree();
    template <typename N>
    void adjustNode(N *node);
    template <typename N>
    void joinNodes(N *node_1, N *node_2, InternalNode *parentNode, int node_1_index);
    template <typename N>
    void redistributeNodes(N *node_1, N *node_2, InternalNode *parentNode, int node_1_index);

private:
    // number of Keys in a block
    // number of child nodes = number of Keys + 1
    int numKeys;
    Node *rootNode;
    void adjustRoot();
};