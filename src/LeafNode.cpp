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

//LeafNode::LeafNode(Disk *disk, int numKeys) : Node(numKeys)
//{
//    this->disk = disk;
//    this->numKeys = numKeys;
//    this->minNumKeys = (numKeys + 1) / 2;
//    this->parentNode = NULL;
//    size = 0;
//    // for (int i = 0; i < numKeys; i++)
//    // {
//    //     this->keys.push_back(0);
//    //     this->childNodes.push_back(NULL);
//    // }
//    this->nextNode = NULL;
//    this->blocks = vector<vector<shared_ptr<Block>>>(1);
//}

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
            return this->blocks[i];
        }
    }
    return {};
}

//Record *LeafNode::getRec(uint32_t key)
//{
//    RecordPointer *recPtr;
//    recPtr = this->getRecPointer(key);
//
//    return this->disk->getRecord(*recPtr);
//    // Block *blk;
//    // blk = recPtr->getBlock();
//    // int offset = recPtr->getOffset();
//
//    // return blk->getRecords()[offset];
//
//    // return this->getRecPointer(key)->getBlock()->getRecords()[this->getRecPointer(key)->getOffset()];
//}
//
//void LeafNode::chainRec(uint32_t key, RecordPointer p)
//{
//    for (int i = 0; i < size; i++)
//    {
//        if (this->keys[i] == key)
//        {
//            Record * currentRec = this->disk->getRecord(p);
//            this->disk->addRecord(currentRec, this->recPointer[i]);
//            this->recPointer[i] = p;
//        }
//    }
//}

int LeafNode::getSize()
{
    return this->keys.size();
}

void LeafNode::insertRec(uint32_t key, <vector<shared_ptr<Block>> blockAddress)
{
    for (int i = 0; i < this->size; i++)
    {
        if (this->keys[i] > key)
        {
            this->keys.insert(keys.begin() + i, key);
            this->blocks.insert(blockAddress.begin() + i, p);
            this->size += 1;
            return;
        }
    }
    this->keys.insert(keys.begin() + size, key);
    this->blocks.insert(blockAddress.begin() + size, p);
    this->size += 1;
    return;
}

LeafNode *LeafNode::split(uint32_t key, <vector<shared_ptr<Block>> b)
{
    this->insertRec(key, b);
    LeafNode *newLeaf = new LeafNode(this->numKeys);
    for (int i = 0; i < (numKeys + 1) / 2; i++)
    {
        uint32_t newKey = *(keys.begin() + numKeys / 2 + 1);
        shared_ptr<Block> newBlk = *(blocks.begin() + numKeys / 2 + 1);
        keys.erase(keys.begin() + numKeys / 2 + 1);
        blocks.erase(blocks.begin() + numKeys / 2 + 1);
        newLeaf->insertRec(newKey, newRec);
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

void LeafNode::moveLastToFrontOf(LeafNode *recipientNode, int orderOfThisChildNode)
{
    uint32_t keyToParent = this->keys.back();
    uint32_t keyToRecipient = keyToParent;
    InternalNode *parentNode = static_cast<InternalNode *>(this->getParent());
    this->keys.erase(this->keys.end());
    parentNode->setKey(0, keyToParent);
    recipientNode->keys.insert(keys.begin(), keyToRecipient);

    printf("this->recPointer size = %d\n",&(this->recPointer.at(0)));
    shared_ptr<Block> child = this->blocks.back();

    this->blocks.erase(this->blocks.end());
    recipientNode->blocks.insert(blocks.begin(), child);
}

void LeafNode::copySingle(uint32_t startKey, uint32_t endKey, vector<Block *> &blocks)
{
    this->keys
}

void LeafNode::printCurrentBlock(uint32_t key, RecordPointer* rp){
    Block* tempB = disk->getBlock(rp->getBlockAddress());
    for(auto r : tempB->getRecords()){
        cout << "Block details: " << r->toString() << endl;
    }
}

void LeafNode::copyStart(uint32_t startKey, vector<Record> &aVector)
{
    bool found = false;
    //cout << this->keys.size() << endl;
    for (auto key : this->keys)
    {
        if (key >= startKey)
        {
            found = true;
        }
        if (found)
        {
            Record *temp = getRec(key);
            aVector.push_back(*temp);
             RecordPointer *rp = this->getRecPointer(key);
            // cout << "Block address: " << rp->getBlockAddress() <<endl;
            // printCurrentBlock(key, rp);
            while(temp->nextRec.getOffset()!=-1 && temp->nextRec.getBlockAddress()!=-1)
            {
                // cout << "Block address: " << (temp->nextRec).getBlockAddress() << "\n" << endl;
                // printCurrentBlock(key, &temp->nextRec);
                temp = this->disk->getRecord(temp->nextRec);
                aVector.push_back(*temp);
            }
        }
    }
}

void LeafNode::copyEnd(uint32_t endKey, vector<Record> &aVector)
{
    bool found = false;
    for (auto key : this->keys)
    {
        if (key > endKey)
        {
            found = true;
            return;
        }
        if (!found)
        {
            Record *temp = getRec(key);
            aVector.push_back(*temp);
            RecordPointer *rp = this->getRecPointer(key);
            // cout << "Block address: " << rp->getBlockAddress() <<endl;
            // printCurrentBlock(key, rp);
            while(temp->nextRec.getOffset()!=-1 && temp->nextRec.getBlockAddress()!=-1){
                // cout << "Block address: " << (temp->nextRec).getBlockAddress() <<endl;
                // printCurrentBlock(key, &temp->nextRec);
                temp = this->disk->getRecord(temp->nextRec);
                aVector.push_back(*temp);
                //std::cout << "key: " << key << " Rating: " << temp->averageRating << endl;
            }
        }
        if (key >= endKey)
        {
            std::cout << "Reach Endkey..." << endl;
            found = true;
            return;
        }
    }
}

void LeafNode::copyRange(vector<Record> &aVector)
{
    for (auto key : this->keys)
    {
        Record *temp = getRec(key);
        aVector.push_back(*temp);
        RecordPointer *rp = this->getRecPointer(key);
        // cout << "Block address: " << rp->getBlockAddress() <<endl;
        // printCurrentBlock(key, rp);
        while (temp->nextRec.getOffset()!=-1 && temp->nextRec.getBlockAddress()!=-1)
        {
            //cout << "Block address: " << (temp->nextRec).getBlockAddress() <<endl;
            //printCurrentBlock(key, &temp->nextRec);
            temp = this->disk->getRecord(temp->nextRec);
            aVector.push_back(*temp);

        }
    }
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
    recipientNode->appendChildNodes(this->blocks);
    // clear nodeTodeDelete
    this->clearKeysAndNodes();
    // std::cout << " " << parentNode->getParent()->toString();
    // printf("%d\n",parentNode->isRoot());
}

void LeafNode::appendChildNodes(vector<shared_ptr<Block>> blocks)
{
    for (shared_ptr<Block> block : blocks)
    {
        this->blocks.insert(this->blocks.begin(), block);
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
