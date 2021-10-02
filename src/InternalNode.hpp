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
    ~InternalNode() override;
    bool isLeafNode() const override;
    // get the current size of internal node
    int getSize();
    bool isRoot() ;
    float getFirstKey() override;
    // helper function for insertion
    Node *getNode(float key);
    InternalNode *split(float key, Node *node);
    void insertChild(float key, Node *childNode);
    void initializeNode(float firstKey, Node *first, Node *second);
    void moveFirstToEndOf(InternalNode* recipientNode);
    void moveLastToFrontOf(InternalNode* recipient, int orderOfThisChildNode);
    void setKey(int index, float key);
    Node* getChildNode(int index);
    int getChildIndex(Node *aNode) const;
    void queueUpChildren(std::queue<Node*>* argQueue);
    Node* firstChild() const;
    std::string toString() const;
    void joinTwoNodes(InternalNode* recipientNode, int nodeToDelete_index,InternalNode* parent);
    vector<float> getAllkeys();
    void removeKey(int index);
    void removeChildNode(int index);
    void appendKeys(vector<float>);
    void appendKeyAt(float key, int index);
    float getKey(int index);
    vector<Node *> getAllChildNodes();
    void insertKeyAtLast(float key);
    void appendChildNodes(vector<Node *>);
private:
    // current number of keys in the node
    int size;
    // maximum number of Keys in the node
    // note that number of child nodes = number of Keys + 1
    int numKeys;
    Node *parentNode;
    // length of order = numNodes + 1
    vector<float> keys;
    // length of childNodes = numNodes
    vector<Node *> childNodes;
};