#pragma once

#include <string>
#include <vector>
using namespace std;
class Node
{
public:
    Node(int numNodes);
    virtual ~Node();
    virtual bool isLeafNode() const = 0;
    virtual uint32_t getFirstKey() = 0;
    virtual int getSize() = 0;
    // virtual bool isRoot() const = 0;
    Node* getParent();
    int getMaxKeySize();
    void replaceFirstKey(uint32_t key);
    void clearKeysAndNodes();
    void setParent(Node*);
    virtual std::string toString() const=0;
    int getNumChildNodes();

private:
    // number of Keys in a block
    // number of child nodes = number of Keys + 1
    int numKeys;
    Node *parentNode;
    // length of order = numNodes + 1
    vector<uint32_t> keys;
    // length of childNodes = numNodes
    vector<Node *> childNodes;
};