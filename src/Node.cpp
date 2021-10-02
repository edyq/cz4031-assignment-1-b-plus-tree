#include <limits.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "Node.hpp"

using namespace std;

Node::Node(int numKeys)
{
    this->numKeys = numKeys;
    this->parentNode = NULL;
}

Node::~Node() {}



Node* Node::getParent(){
    return this->parentNode;
}


int Node::getMaxKeySize()
{
    return this->numKeys;
}

void Node::replaceFirstKey(uint32_t key)
{
    this->keys[0] = key;
}


void Node::clearKeysAndNodes()
{
    this->keys.clear();
    this->childNodes.clear();
}

void Node::setParent(Node* parent)
{
    this->parentNode = parent;
}


int Node::getNumChildNodes()
{
    return this->childNodes.size();
}
