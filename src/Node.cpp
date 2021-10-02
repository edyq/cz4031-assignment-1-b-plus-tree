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
    // for (int i = 0; i < numKeys; i++)
    // {
    //     this->keys.push_back(0);
    //     this->childNodes.push_back(NULL);
    // }
    // this->childNodes.push_back(NULL);
}

Node::~Node() {}



Node* Node::getParent(){
    return this->parentNode;
}


int Node::getMaxKeySize()
{
    return this->numKeys;
}

void Node::replaceFirstKey(float key)
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
