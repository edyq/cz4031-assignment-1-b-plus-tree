#include <limits.h>
#include <string>
#include <vector>
#include <sstream>
#include "LeafNode.hpp"
#include "block.h"

using namespace std;

LeafNode::LeafNode(int numKeys) : Node(numKeys)
{
    this->numKeys = numKeys;
    this->minNumKeys = (numKeys + 1) / 2;
    this->parentNode = NULL;
    size = 0;
    // for (int i = 0; i < numKeys; i++)
    // {
    //     this->keys.push_back(0);
    //     this->childNodes.push_back(NULL);
    // }
    this->nextNode = NULL;
    this->blocks = vector<vector<shared_ptr<Block>>>(1);
}

bool LeafNode::isLeafNode() const
{
    return true;
}

LeafNode::~LeafNode()
{
}

//RecordPointer *LeafNode::getRecPointer(uint32_t key)
vector<shared_ptr<Block>> *LeafNode::getBlock(uint32_t key)
{
    for (int i = 0; i < size; i++)
    {
        if (this->keys[i] == key)
        {
            return this->blocks[i];  // TODO: error
        }
    }
    return {};
}

int LeafNode::getSize()
{
    return this->keys.size();
}

void LeafNode::insertRec(uint32_t key, vector<shared_ptr<Block>> blockAddress)
{
    for (int i = 0; i < this->size; i++)
    {
        if (this->keys[i] > key)
        {
            this->keys.insert(keys.begin() + i, key);
            this->blocks.insert(blockAddress.begin() + i, p);  // TODO: p does not exist
            this->size += 1;
            return;
        }
    }
    this->keys.insert(keys.begin() + size, key);
    this->blocks.insert(blockAddress.begin() + size, p);  // TODO: p!!!
    this->size += 1;
    return;
}

LeafNode *LeafNode::split(uint32_t key, vector<shared_ptr<Block>> b)
{
    this->insertRec(key, b);
    LeafNode *newLeaf = new LeafNode(this->numKeys);
    for (int i = 0; i < (numKeys + 1) / 2; i++)
    {
        uint32_t newKey = *(keys.begin() + numKeys / 2 + 1);
        shared_ptr<Block> newBlk = *(blocks.begin() + numKeys / 2 + 1);
        keys.erase(keys.begin() + numKeys / 2 + 1);
        blocks.erase(blocks.begin() + numKeys / 2 + 1);
        newLeaf->insertRec(newKey, newRec);  // TODO: where is newRec?
    }
    this->size = numKeys / 2 + 1;
    return newLeaf;
}

void LeafNode::setNextNode(LeafNode *nextNode)
{
    this->nextNode = nextNode;
}
LeafNode *LeafNode::getNextNode()
{
    return this->nextNode;
}

uint32_t LeafNode::getFirstKey()
{
    if (this->keys.empty())
    {
        return -1.0;
    }
    return this->keys[0];
}

void LeafNode::removeRec(uint32_t key)
{
    int curr_pos = 0;
    int end_pos = keys.size();
    while (curr_pos != end_pos && keys.at(curr_pos) != key)
    {
        curr_pos++;
    }
    if (curr_pos == end_pos)
    {
        printf("Key %.1f not found at leaf node", key);
        return;
    }
    keys.erase(keys.begin() + curr_pos);
    blocks.erase(blocks.begin() + curr_pos);
}

int LeafNode::getMinNumKeys()
{
    return this->minNumKeys;
}

void LeafNode::moveFirstToEndOf(LeafNode *recipientNode)
{
    uint32_t keyToRecipient = this->keys.at(0);
    this->keys.erase(this->keys.begin());
    uint32_t keyToParent = this->keys.at(0);
    InternalNode *parentNode = static_cast<InternalNode *>(this->getParent());
    parentNode->setKey(0, keyToParent);
    recipientNode->keys.push_back(keyToRecipient);

    shared_ptr<Block> child = this->blocks.at(0);
    this->blocks.erase(this->blocks.begin());
    recipientNode-blocks.push_back(child);
}

// TODO: FIX ME
void LeafNode::moveLastToFrontOf(LeafNode *recipientNode, int orderOfThisChildNode)
{
    uint32_t keyToParent = this->keys.back();
    uint32_t keyToRecipient = keyToParent;
    InternalNode *parentNode = static_cast<InternalNode *>(this->getParent());
    this->keys.erase(this->keys.end());
    parentNode->setKey(0, keyToParent);
    recipientNode->keys.insert(keys.begin(), keyToRecipient);

    printf("this->recPointer size = %d\n",&(this->recPointer.at(0)));  // TODO: error
    shared_ptr<Block> child = this->blocks.back();  // TODO: error

    this->blocks.erase(this->blocks.end());
    recipientNode->blocks.insert(blocks.begin(), child);
}

bool LeafNode::isRoot() 
{
    return !(this->getParent());
}

std::string LeafNode::toString() const
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

void LeafNode::joinTwoNodes(LeafNode *recipientNode, int nodeToDelete_index, InternalNode *parentNode)
{
    printf("Entered Leaf Node joinTwoNodes\n");
    // add the nodeToDelete's keys to the end of recipient keys
    recipientNode->appendKeys(this->getAllkeys());
    // add the nodeToDelete's childnodes to the end of recipient childnodes
    recipientNode->appendChildNodes(this->blocks);  // TODO:  error: no viable conversion from 'vector<vector<shared_ptr<Block>>>' to 'vector<shared_ptr<Block>>'
    // clear nodeTodeDelete
    this->clearKeysAndNodes();
    // std::cout << " " << parentNode->getParent()->toString();
    // printf("%d\n",parentNode->isRoot());
}

void LeafNode::appendChildNodes(vector<shared_ptr<Block>> blocks)
{
    for (shared_ptr<Block> block : blocks)
    {
    	//        this->blocks.insert(this->blocks.begin(), block); // TODO: error: no matching member function for call to 'insert'
    }
}

vector<uint32_t> LeafNode::getAllkeys()
{
    return this->keys;
}

void LeafNode::removeKey(int index)
{
    this->keys.erase(this->keys.begin() + index);
}

void LeafNode::removeChildNode(int index)
{
    this->keys.erase(this->keys.begin() + index - 1);
    this->blocks.erase(this->blocks.begin() + index);
}

void LeafNode::appendKeys(vector<uint32_t> newKeys)
{
    for (uint32_t newKey : newKeys)
    {
        this->keys.push_back(newKey);
    }
}

void LeafNode::appendKeyAt(uint32_t key, int index)
{
    if (this->keys.size() >= this->numKeys)
    {
        this->keys.insert(this->keys.begin() + index, key);
    }
}

uint32_t LeafNode::getKey(int index)
{
    return this->keys.at(index);
}

//vector<RecordPointer> LeafNode::getAllRecPtrs()
//{
//    return this->recPointer;
//}
//
//vector<Record *> LeafNode::getAllRecs()
//{
//    vector<RecordPointer> allRecPtr;
//    vector<Record *> allRec;
//    allRecPtr = this->recPointer;
//
//    for (auto recPtr : allRecPtr)
//    {
//        Block *blk;
//        int offset;
//        Record *rec;
//
//        // blk = recPtr.getBlock();
//        // offset = recPtr.getOffset();
//        // rec = blk->getRecords()[offset];
//        rec = this->disk->getRecord(recPtr);
//
//        allRec.push_back(rec);
//    }
//
//    return allRec;
//}
