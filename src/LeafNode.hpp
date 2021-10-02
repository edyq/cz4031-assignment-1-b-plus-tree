#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <iostream>
#include "Node.hpp"
#include "InternalNode.hpp"
#include "RecordPointer.hpp"
#include "Record.hpp"
#include "Disk.hpp"
using namespace std;

class LeafNode : public Node
{
public:
    LeafNode(int numNodes);
//    LeafNode(Disk *disk, int numNodes);
    ~LeafNode() override;
    bool isLeafNode() const override;
    int getSize();
    uint32_t getFirstKey() override;
    bool isRoot();
    // helper function for insertion
    void insertRec(uint32_t key, shared_ptr<Block> blockAddress);
//    void insertRec(uint32_t key, RecordPointer p);
    LeafNode *split(uint32_t key, shared_ptr<Block> b);
//    LeafNode *split(uint32_t key, RecordPointer p);
    void setNextNode(LeafNode *nextNode);
//    void copySingle(uint32_t startKey, uint32_t endKey, vector<Record> &aVector);
//    void copyStart(uint32_t aKey, vector<Record> &aVector);
//    void copyEnd(uint32_t aKey, vector<Record> &aVector);
//    void copyRange(vector<Record> &aVector);
    LeafNode *getNextNode();
    void removeRec(uint32_t key);
    int getMinNumKeys();
    void moveFirstToEndOf(LeafNode *recipientNode);
    void moveLastToFrontOf(LeafNode *recipient, int orderOfThisChildNode);
    std::string toString() const;
    void joinTwoNodes(LeafNode *recipientNode, int nodeToDelete_index, InternalNode *parent);
    void appendChildNodes(vector<shared_ptr<Block>> blocks);
//    void appendChildNodes(vector<RecordPointer> records);
    vector<uint32_t> getAllkeys();
    void removeKey(int index);
    void removeChildNode(int index);
    void appendKeys(vector<uint32_t>);
    void appendKeyAt(uint32_t key, int index);
//    void printCurrentBlock(uint32_t key, RecordPointer* rp);
    uint32_t getKey(int index);
//    RecordPointer *getRecPointer(uint32_t key);
    <vector<shared_ptr<Block>> *getBlock(uint32_t key);
//    Record *getRec(uint32_t key);
//    vector<RecordPointer> getAllRecPtrs();
//    vector<Record *> getAllRecs();
//    void chainRec(uint32_t key, RecordPointer p);
    
private:
    // maximum number of Keys in a block
    // note that number of record nodes = number of Keys
    int numKeys;
    // minimum numebr of keys in a block
    int minNumKeys;
    // current number of keys in the node
    int size;
    Node *parentNode;
    // length of order = numNodes + 1
    vector<uint32_t> keys;
    // length of childNodes = numNodes
    vector<Record *> childNodes; //No longer fits the requirement later change to below
//    vector<RecordPointer> recPointer;
    vector<vector<shared_ptr<Block>>> blocks;
    LeafNode *nextNode;
//    Disk *disk;
};