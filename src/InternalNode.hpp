#pragma once

#include <string>
#include <vector>
#include <queue>
#include "Node.hpp"
// #include "LeafNode.hpp"
using namespace std;

class InternalNode : public Node
{
public:
    InternalNode(int numNodes);
    ~InternalNode();
    bool isLeafNode() const;
    // get the current size of internal node
    int getSize();
    bool isRoot() ;
    uint32_t getFirstKey();
    // helper function for insertion
    Node *getNode(uint32_t key);
    InternalNode *split(uint32_t key, Node *node);
    void insertChild(uint32_t key, Node *childNode);
    void initializeNode(uint32_t firstKey, Node *first, Node *second);
    void moveFirstToEndOf(InternalNode* recipientNode);
    void moveLastToFrontOf(InternalNode* recipient, int orderOfThisChildNode);
    void setKey(int index, uint32_t key);
    Node* getChildNode(int index);
    int getChildIndex(Node *aNode) const;
    void queueUpChildren(std::queue<Node*>* argQueue);
    Node* firstChild() const;
    std::string toString() const;
    void joinTwoNodes(InternalNode* recipientNode, int nodeToDelete_index,InternalNode* parent);
    vector<uint32_t> getAllkeys();
    void removeKey(int index);
    void removeChildNode(int index);
    void appendKeys(vector<uint32_t>);
    void appendKeyAt(uint32_t key, int index);
    uint32_t getKey(int index);
    vector<Node *> getAllChildNodes();
    void insertKeyAtLast(uint32_t key);
    void appendChildNodes(vector<Node *>);
private:
    // current number of keys in the node
    int size;
    // maximum number of Keys in the node
    // note that number of child nodes = number of Keys + 1
    int numKeys;
    Node *parentNode;
    // length of order = numNodes + 1
    vector<uint32_t> keys;
    // length of childNodes = numNodes
    vector<Node *> childNodes;
};