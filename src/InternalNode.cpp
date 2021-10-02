#include <limits.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include "InternalNode.hpp"

using namespace std;

InternalNode::InternalNode(int numKeys) : Node(numKeys)
{
    this->size = 0;
    this->numKeys = numKeys;
    this->parentNode = NULL;
}

bool InternalNode::isLeafNode() const
{
    return false;
}

InternalNode::~InternalNode()
{
}

Node *InternalNode::getNode(uint32_t key)
{
    if (key < this->keys[0])
    {
        return childNodes[0];
    }
    for (int i = 1; i < size; i++)
    {
        if (key < this->keys[i])
        {
            return childNodes[i];
        }
    }
    return childNodes[size];
}

int InternalNode::getSize()
{
    return this->keys.size();
}

uint32_t InternalNode::getFirstKey()
{
    return this->childNodes[0]->getFirstKey();
}

InternalNode *InternalNode::split(uint32_t key, Node *node)
{
    // cout << "spliting internal node" << endl;
    this->insertChild(key, node);
    InternalNode *newNode = new InternalNode(numKeys);
    this->childNodes[(numKeys + 1) / 2 + 1]->setParent(newNode);
    this->childNodes[(numKeys + 1) / 2 + 2]->setParent(newNode);
    newNode->initializeNode(this->keys[(numKeys + 1) / 2 + 1], this->childNodes[(numKeys + 1) / 2 + 1], this->childNodes[(numKeys + 1) / 2 + 2]);
    for (int i = 0; i < (numKeys) / 2 + 1; i++)
    {
        uint32_t newKey = *(keys.begin() + (numKeys + 1) / 2);
        Node *newChild = *(childNodes.begin() + (numKeys + 1) / 2 + 1);
        keys.erase(keys.begin() + (numKeys + 1) / 2);
        childNodes.erase(childNodes.begin() + (numKeys + 1) / 2 + 1);
        if (i > 1)
        {
            newChild->setParent(newNode);
            newNode->insertChild(newKey, newChild);
        }
    }
    this->size = (numKeys + 1) / 2;
    return newNode;
}

void InternalNode::insertChild(uint32_t key, Node *childNode)
{
    // printf("inserting child node with first key %.1f into node\n", key);
    for (int i = 0; i < this->size; i++)
    {
        if (this->keys[i] > key)
        {
            this->keys.insert(keys.begin() + i, key);
            this->childNodes.insert(childNodes.begin() + i + 1, childNode);
            this->size += 1;
            return;
        }
    }
    this->keys.insert(keys.begin() + size, key);
    this->childNodes.insert(childNodes.begin() + size + 1, childNode);
    this->size += 1;
    return;
}

void InternalNode::initializeNode(uint32_t firstKey, Node *first, Node *second)
{
    this->keys.push_back(firstKey);
    this->childNodes.push_back(first);
    this->childNodes.push_back(second);
    this->size = 1;
    // LeafNode *secondChild = static_cast<LeafNode *>(this->childNodes[1]);
    // printf("first key of second childNode %.1f\n", secondChild->getFirstKey());
}

void InternalNode::moveFirstToEndOf(InternalNode *recipientNode)
{
    int keyToParent = this->keys.at(0);
    InternalNode *parentNode = static_cast<InternalNode *>(this->getParent());
    int keyToRecipient = parentNode->getKey(0);
    this->keys.erase(this->keys.begin());
    parentNode->setKey(0, keyToParent);
    recipientNode->keys.push_back(keyToRecipient);

    Node *child = this->childNodes.at(0);
    this->childNodes.erase(this->childNodes.begin());
    recipientNode->childNodes.push_back(child);
}

void InternalNode::setKey(int index, uint32_t key)
{
    this->keys.at(index) = key;
}

void InternalNode::moveLastToFrontOf(InternalNode *recipientNode, int orderOfThisChildNode)
{
    uint32_t keyToParent = this->keys.back();
    InternalNode *parentNode = static_cast<InternalNode *>(this->getParent());
    uint32_t keyToRecipient = parentNode->getKey(orderOfThisChildNode - 1);
    this->keys.erase(this->keys.end());
    parentNode->setKey(orderOfThisChildNode - 1, keyToParent);
    recipientNode->keys.insert(keys.begin(), keyToRecipient);

    Node *child = this->childNodes.back();
    this->childNodes.erase(this->childNodes.end());
    recipientNode->childNodes.insert(childNodes.begin(), child);
}

Node *InternalNode::getChildNode(int index)
{
    return this->childNodes.at(index);
}


int InternalNode::getChildIndex(Node* childNode) const
{
    Node* trythis = this->childNodes[0];
    for (int i = 0; i < this->childNodes.size(); i++) {
        if (this->childNodes[i] == childNode) {
            return i;
        }
    }
    printf("Node is not a childnode of its parent node. Error.");
}

void InternalNode::queueUpChildren(std::queue<Node *>* argQueue)

{
    for (auto node : childNodes)
    {
        argQueue->push(node);
    }
}

Node *InternalNode::firstChild() const
{
    return childNodes.front();
}

std::string InternalNode::toString() const
{
    std::ostringstream keyToTextConverter;

    bool first = true;
    for (auto key : this->keys)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            keyToTextConverter << " ";
        }
        keyToTextConverter << key;
    }

    return keyToTextConverter.str();
}

void InternalNode::joinTwoNodes(InternalNode* recipientNode, int nodeToDelete_index,InternalNode* parent)
{
    printf("Entered Internal Node joinTwoNodes\n");
    // InternalNode* parent = static_cast<InternalNode*>(this->parentNode);
    // add parent key to the end of recipient keys
    // printf("parent key appended = %d\n", parent->getKey(nodeToDelete_index-1));
    recipientNode->insertKeyAtLast(parent->getKey(nodeToDelete_index-1));
    // for(int key: recipientNode->getAllkeys())
    // {
    //     printf("jointwonodes rec key = %d\n",key);
    // }
    // add the nodeToDelete's keys to the end of recipient keys
    recipientNode->appendKeys(this->getAllkeys());
    // add the nodeToDelete's childnodes to the end of recipient childnodes
    recipientNode->appendChildNodes(this->getAllChildNodes());
    // update childnodes' parent
    for (Node* child : recipientNode->childNodes)
    {
        child->setParent(recipientNode);
    }
    // std::cout << " " << parent->getParent()->toString();
    // clear nodeTodeDelete
    this->clearKeysAndNodes();
}

vector<uint32_t> InternalNode::getAllkeys()
{
    return this->keys;
}
void InternalNode::removeKey(int index)
{
    this->keys.erase(this->keys.begin()+index)-1;
}

void InternalNode::removeChildNode(int index)
{
    this->childNodes.erase(this->childNodes.begin()+index);
    // printf("Updated size of parent childnodes = %d\n", this->childNodes.size());
}

bool InternalNode::isRoot()
{
    return !(this->getParent());
}

void InternalNode::appendKeys(vector<uint32_t> newKeys)
{
    for(uint32_t newKey : newKeys)
    {   
        this->keys.push_back(newKey);
    }
}

void InternalNode::appendKeyAt(uint32_t key, int index)
{
    if(this->keys.size()>=this->numKeys)
    {
        this->keys.insert(this->keys.begin()+index, key);
    }
}

uint32_t InternalNode::getKey(int index)
{
    return this->keys.at(index);
}

vector<Node *> InternalNode::getAllChildNodes()
{
    return this->childNodes;
}

void InternalNode::insertKeyAtLast(uint32_t key)
{
    this->keys.push_back(key);
}

void InternalNode::appendChildNodes(vector<Node *> newNodes)
{
    for(auto newNode : newNodes)
    {   
        this->childNodes.push_back(newNode);
    }
}