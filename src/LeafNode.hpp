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
    float getFirstKey() override;
    bool isRoot();
    // helper function for insertion
    void insertRec(float key, shared_ptr<Block> blockAddress);
//    void insertRec(float key, RecordPointer p);
    LeafNode *split(float key, shared_ptr<Block> b);
//    LeafNode *split(float key, RecordPointer p);
    void setNextNode(LeafNode *nextNode);
//    void copySingle(float startKey, float endKey, vector<Record> &aVector);
//    void copyStart(float aKey, vector<Record> &aVector);
//    void copyEnd(float aKey, vector<Record> &aVector);
//    void copyRange(vector<Record> &aVector);
    LeafNode *getNextNode();
    void removeRec(float key);
    int getMinNumKeys();
    void moveFirstToEndOf(LeafNode *recipientNode);
    void moveLastToFrontOf(LeafNode *recipient, int orderOfThisChildNode);
    std::string toString() const;
    void joinTwoNodes(LeafNode *recipientNode, int nodeToDelete_index, InternalNode *parent);
    void appendChildNodes(vector<shared_ptr<Block>> blocks);
//    void appendChildNodes(vector<RecordPointer> records);
    vector<float> getAllkeys();
    void removeKey(int index);
    void removeChildNode(int index);
    void appendKeys(vector<float>);
    void appendKeyAt(float key, int index);
//    void printCurrentBlock(float key, RecordPointer* rp);
    float getKey(int index);
//    RecordPointer *getRecPointer(float key);
    <vector<shared_ptr<Block>> *getBlock(float key);
    Record *getRec(float key);
//    vector<RecordPointer> getAllRecPtrs();
//    vector<Record *> getAllRecs();
//    void chainRec(float key, RecordPointer p);
    
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
    vector<float> keys;
    // length of childNodes = numNodes
    vector<Record *> childNodes; //No longer fits the requirement later change to below
//    vector<RecordPointer> recPointer;
    vector<vector<shared_ptr<Block>>> blocks;
    LeafNode *nextNode;
//    Disk *disk;
};