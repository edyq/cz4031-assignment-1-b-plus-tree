#include <limits.h>
#include <string>
#include <vector>
#include "BPlusTree.hpp"
#include <iostream>
#include <queue>
#include "block.h"
using namespace std;

BPlusTree::BPlusTree(Disk *disk, int numKeys)
{
    this->disk = disk;
    this->numKeys = numKeys;
    this->rootNode = new LeafNode(disk, numKeys);
}

void BPlusTree::insert(float key, RecordPointer p)
{
    // printf("inserting record with key %.1f into leaf node\n", key);
    LeafNode *leaf = findLeafNode(key);
    // printf("found leafNode starting key %.1f\n", leaf->getFirstKey());
    RecordPointer *pos = leaf->getRecPointer(key);
    if (pos)
    {
        leaf->chainRec(key, p);
    }
    else
    {
        if (leaf->getSize() == numKeys)
        {
            // printf("Spliting leaf node\n");
            LeafNode *newNode = leaf->split(key, p);
            newNode->setNextNode(leaf->getNextNode());
            leaf->setNextNode(newNode);
            float firstKey = newNode->getFirstKey();
            InternalNode *currentParent = static_cast<InternalNode *>(leaf->getParent());
            if (currentParent == NULL)
            {
                // printf("Creating a new parent node first key %.1f\n", firstKey);
                this->rootNode = new InternalNode(this->numKeys);
                currentParent = static_cast<InternalNode *>(this->rootNode);
                leaf->setParent(currentParent);
                newNode->setParent(currentParent);
                currentParent->initializeNode(firstKey, leaf, newNode);
            }
            else
            {
                newNode->setParent(currentParent);
                insertInternal(currentParent, firstKey, newNode);
            }
        }
        else
        {
            leaf->insertRec(key, p);
        }
    }
}

void BPlusTree::remove(float key)
{
    if (!rootNode)
    {
        printf("Empty tree. Remove failed.\n");
        return;
    }
    LeafNode *leafNode = findLeafNode(key);
    if (!leafNode)
    {
        printf("Key does not exist. Remove failed.\n");
        return;
    }
    else if (!leafNode->getRec(key))
    {
        printf("No record found for this key. Remove failed.\n");
        return;
    }
    printf("Found key\n");
    leafNode->removeRec(key);

    if (leafNode->getSize() < leafNode->getMinNumKeys())
    {
        printf("Adjusting leafnode.\n");
        adjustNode(leafNode);
    }
}

LeafNode *BPlusTree::findLeafNode(float key)
{

    Node *node = rootNode;
    //cout << "Visited node: " << node->toString() << endl;
    while (!node->isLeafNode())
    {
        InternalNode *internalNode = static_cast<InternalNode *>(node);
        node = internalNode->getNode(key);
        //cout << "Visited node: " << node->toString() << endl;
    }
    return static_cast<LeafNode *>(node);
}

vector<Block *> BPlusTree::searchRecord(float startKey, float endKey)
{
    printf("Start searching records...\n");
    auto startLeaf = findLeafNode(startKey);
    printf("Found start leaf\n");
    auto endLeaf = findLeafNode(endKey);
    printf("Found end leaf\n");
    vector<Block *> results;
    if (!startLeaf || !endLeaf)
    {
        /* To Print None */
        printf("Not found startLeaf or endLeaf...\n");
        return results;
    }

    do {
    	auto keys = startLeaf->getAllkeys();
    	for (auto key : keys) {
    		auto startLeaf->getBlock(key);
    	}
    	startLeaf = startLeaf->getNextNode();
    } while (startLeaf != endLeaf);
    return results;
}

void BPlusTree::insertInternal(InternalNode *parentNode, float key, Node *childNode)
{
    if (parentNode->getSize() == numKeys)
    {
        InternalNode *newNode = parentNode->split(key, childNode);
        float firstKey = newNode->getFirstKey();
        InternalNode *currentParent = static_cast<InternalNode *>(parentNode->getParent());
        if (currentParent == NULL)
        {
            // printf("Creating a new parent node first key %.1f\n", firstKey);
            this->rootNode = new InternalNode(this->numKeys);
            currentParent = static_cast<InternalNode *>(this->rootNode);
            parentNode->setParent(currentParent);
            newNode->setParent(currentParent);
            currentParent->initializeNode(firstKey, parentNode, newNode);
        }
        else
        {
            newNode->setParent(currentParent);
            insertInternal(currentParent, firstKey, newNode);
        }
    }
    else
    {
        parentNode->insertChild(key, childNode);
    }
}

bool BPlusTree::isEmptyTree()
{
    return !rootNode;
}

template <typename N>
void BPlusTree::adjustNode(N *node)
{
        node->toString();
    if (node->isRoot())
    {
        std::cout << "" << node->toString();
        printf("Adjusting root node.\n");
        adjustRoot();
        return;
    }

    auto parentNode = static_cast<InternalNode *>(node->getParent());

    int orderOfThisChildNode = parentNode->getChildIndex(node);
    N *neighborNode = (orderOfThisChildNode == 0) ? static_cast<N *>(parentNode->getChildNode(1)) : static_cast<N *>(parentNode->getChildNode(orderOfThisChildNode - 1));

    if (node->getSize() + neighborNode->getSize() <= neighborNode->getMaxKeySize())
    {
        if (orderOfThisChildNode == 0)
        {
            printf("Joining with neighbour nodes (right)\n");
            joinNodes(node, neighborNode, parentNode, 1);
        }
        else
        {
            printf("Joining with neighbour nodes (left)\n");
            joinNodes(neighborNode, node, parentNode, orderOfThisChildNode);
        }
    }
    else
    {
        printf("Redistributing with neighbour nodes\n");
        redistributeNodes(neighborNode, node, parentNode, orderOfThisChildNode);
    }
}

template <typename N>
void BPlusTree::joinNodes(N *recipient, N *nodeToDelete, InternalNode *parentNode, int nodeToDelete_index)
{

    nodeToDelete->joinTwoNodes(recipient, nodeToDelete_index, parentNode);
    //for parent, remove this key and childnode
    parentNode->removeChildNode(nodeToDelete_index);
    parentNode->removeKey(nodeToDelete_index - 1);
    printf("Done joining nodes\n");
    printf("Temporary tree:\n");
    this->printTree();

    // if(parentNode->getAllChildNodes().size()==1 && parentNode->getAllkeys().size() == 0)
    // {
    //     recipient->appendKeyAt(parentNode->getKey(0),nodeToDelete_index);
    //     this->rootNode = recipient;
    // }
    // else{
    //     parentNode->removeKey(nodeToDelete_index-1);
    // }
    // for(int key: recipient->getAllkeys())
    // {
    //     printf("rec key = %d\n",key);
    // }
    // printf("Parent node key size = %d\n",parentNode->getSize());
    // printf("Parent node min key size = %d\n",parentNode->getMaxKeySize()/2);
    if (parentNode->getSize() < (parentNode->getMaxKeySize() / 2))
    {
        printf("Adjusting parent node\n");
        adjustNode(parentNode);
    }
    delete nodeToDelete;
}

template <typename N>
void BPlusTree::redistributeNodes(N *sender, N *recipient, InternalNode *parentNode, int reciIndex)
{
    if (reciIndex == 0)
    {
        sender->moveFirstToEndOf(recipient);
    }
    else
    {
        sender->moveLastToFrontOf(recipient, reciIndex);
    }
}

void BPlusTree::adjustRoot()
{
    InternalNode *rootNodeInternal = static_cast<InternalNode *>(rootNode);
    if (!rootNode->isLeafNode() && rootNode->getSize() == 1)
    {
  printf("Entered adjustRoot if, discarding root node\n");
        auto discardedNode = static_cast<InternalNode *>(rootNode);
        auto newRoot = static_cast<InternalNode *>(rootNode)->getChildNode(0);
        newRoot->setParent(nullptr);
        // added this
        this->rootNode = newRoot;
        //
        delete discardedNode;
    }
    else if (!rootNode->getSize() && rootNodeInternal->getAllChildNodes().size() == 1)
    {
          printf("Root node empty key but one child node, discarding root node\n");

        this->rootNode = rootNodeInternal->getAllChildNodes().at(0);
    }

}

// Set of print methods
void BPlusTree::printTree()
{
    if (!rootNode)
    {
        printEmptyTree();
    }
    else
    {
        printNoneEmptyTree();
    }
}

void BPlusTree::printLeaves()
{
    if (!rootNode)
    {
        printEmptyTree();
        return;
    }
    auto node = rootNode;
    while (!node->isLeafNode())
    {
        node = static_cast<InternalNode *>(node)->firstChild();
    }
    auto leafNode = static_cast<LeafNode *>(node);
    while (leafNode)
    {
        std::cout << "| " << leafNode->toString() << " ";
        leafNode = leafNode->getNextNode();
    }
    std::cout << "|" << std::endl;
}

void BPlusTree::printEmptyTree()
{
    std::cout << "Empty tree." << std::endl;
}

void BPlusTree::printNoneEmptyTree()
{
    std::queue<Node *> queue0;
    std::queue<Node *> queue1;

    auto currentRank = &queue0;
    auto nextRank = &queue1;

    currentRank->push(rootNode);
    while (!currentRank->empty())
    {
        printCurrentRank(currentRank, nextRank);
        auto tmp = currentRank;
        currentRank = nextRank;
        nextRank = tmp;
    }
}

void BPlusTree::printCurrentRank(std::queue<Node *> *argCurrentRank, std::queue<Node *> *argNextRank)
{
    std::cout << "|";
    while (!argCurrentRank->empty())
    {
        Node *currentNode = argCurrentRank->front();
        std::cout << " " << currentNode->toString();
        std::cout << " |";

        if (!currentNode->isLeafNode())
        {
            auto internalNode = static_cast<InternalNode *>(currentNode);
            internalNode->queueUpChildren(argNextRank);
        }

        argCurrentRank->pop();
    }

    std::cout << std::endl;
}
