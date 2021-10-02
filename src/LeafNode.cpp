#include <limits.h>
#include <string>
#include <vector>
#include <sstream>
#include "LeafNode.hpp"

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
}

LeafNode::LeafNode(Disk *disk, int numKeys) : Node(numKeys)
{
    this->disk = disk;
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
}

bool LeafNode::isLeafNode() const
{
    return true;
}

LeafNode::~LeafNode()
{
}

RecordPointer *LeafNode::getRecPointer(float key)
{
    for (int i = 0; i < size; i++)
    {
        if (this->keys[i] == key)
        {
            return &(this->recPointer[i]);
        }
    }
    return NULL;
}

Record *LeafNode::getRec(float key)
{
    RecordPointer *recPtr;
    recPtr = this->getRecPointer(key);

    return this->disk->getRecord(*recPtr);
    // Block *blk;
    // blk = recPtr->getBlock();
    // int offset = recPtr->getOffset();

    // return blk->getRecords()[offset];

    // return this->getRecPointer(key)->getBlock()->getRecords()[this->getRecPointer(key)->getOffset()];
}

void LeafNode::chainRec(float key, RecordPointer p)
{
    for (int i = 0; i < size; i++)
    {
        if (this->keys[i] == key)
        {
            Record * currentRec = this->disk->getRecord(p);
            this->disk->addRecord(currentRec, this->recPointer[i]);
            this->recPointer[i] = p;
        }
    }
}

int LeafNode::getSize()
{
    return this->keys.size();
}

void LeafNode::insertRec(float key, RecordPointer p)
{
    for (int i = 0; i < this->size; i++)
    {
        if (this->keys[i] > key)
        {
            this->keys.insert(keys.begin() + i, key);
            this->recPointer.insert(recPointer.begin() + i, p);
            this->size += 1;
            return;
        }
    }
    this->keys.insert(keys.begin() + size, key);
    this->recPointer.insert(recPointer.begin() + size, p);
    this->size += 1;
    return;
}

LeafNode *LeafNode::split(float key, RecordPointer p)
{
    this->insertRec(key, p);
    LeafNode *newLeaf = new LeafNode(this->disk, this->numKeys);
    for (int i = 0; i < (numKeys + 1) / 2; i++)
    {
        float newKey = *(keys.begin() + numKeys / 2 + 1);
        RecordPointer newRec = *(recPointer.begin() + numKeys / 2 + 1);
        keys.erase(keys.begin() + numKeys / 2 + 1);
        recPointer.erase(recPointer.begin() + numKeys / 2 + 1);
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

float LeafNode::getFirstKey()
{
    if (this->keys.empty())
    {
        return -1.0;
    }
    return this->keys[0];
}

void LeafNode::removeRec(float key)
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
    recPointer.erase(recPointer.begin() + curr_pos);
}

int LeafNode::getMinNumKeys()
{
    return this->minNumKeys;
}

void LeafNode::moveFirstToEndOf(LeafNode *recipientNode)
{
    float keyToRecipient = this->keys.at(0);
    this->keys.erase(this->keys.begin());
    float keyToParent = this->keys.at(0);
    InternalNode *parentNode = static_cast<InternalNode *>(this->getParent());
    parentNode->setKey(0, keyToParent);
    recipientNode->keys.push_back(keyToRecipient);

    RecordPointer child = this->recPointer.at(0);
    this->recPointer.erase(this->recPointer.begin());
    recipientNode->recPointer.push_back(child);
}

void LeafNode::moveLastToFrontOf(LeafNode *recipientNode, int orderOfThisChildNode)
{
    float keyToParent = this->keys.back();
    float keyToRecipient = keyToParent;
    InternalNode *parentNode = static_cast<InternalNode *>(this->getParent());
    this->keys.erase(this->keys.end());
    parentNode->setKey(0, keyToParent);
    recipientNode->keys.insert(keys.begin(), keyToRecipient);

    printf("this->recPointer size = %d\n",&(this->recPointer.at(0)));
    RecordPointer child = this->recPointer.back();

    this->recPointer.erase(this->recPointer.end());
    recipientNode->recPointer.insert(recPointer.begin(), child);
}

void LeafNode::copySingle(float startKey, float endKey, vector<Record> &aVector)
{
    bool found = false;
    //cout << this->keys.size() << endl;
    for (auto key : this->keys)
    {
        if (key >= startKey && key <= endKey)
        {
            found = true;
        }
        else
            found = false;
        if (found)
        {
            Record *temp = getRec(key);
            aVector.push_back(*temp);
            RecordPointer *rp = this->getRecPointer(key);
            // cout << "Block address: " << rp->getBlockAddress() <<endl;
            // printCurrentBlock(key, rp);
            while (temp->nextRec.getOffset()!=-1 && temp->nextRec.getBlockAddress()!=-1)
            {
                // cout << "Block address: " << (temp->nextRec).getBlockAddress() <<endl;
                // printCurrentBlock(key, &temp->nextRec);
                temp = this->disk->getRecord(temp->nextRec);
                aVector.push_back(*temp);
            }
        }
    }
}

void LeafNode::printCurrentBlock(float key, RecordPointer* rp){
    Block* tempB = disk->getBlock(rp->getBlockAddress());
    for(auto r : tempB->getRecords()){
        cout << "Block details: " << r->toString() << endl;
    }
}

void LeafNode::copyStart(float startKey, vector<Record> &aVector)
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

void LeafNode::copyEnd(float endKey, vector<Record> &aVector)
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
    recipientNode->appendChildNodes(this->recPointer);
    // clear nodeTodeDelete
    this->clearKeysAndNodes();
    // std::cout << " " << parentNode->getParent()->toString();
    // printf("%d\n",parentNode->isRoot());
}

void LeafNode::appendChildNodes(vector<RecordPointer> records)
{
    for (RecordPointer record : records)
    {
        this->recPointer.insert(this->recPointer.begin(), record);
    }
}

vector<float> LeafNode::getAllkeys()
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
    this->recPointer.erase(this->recPointer.begin() + index);
}

void LeafNode::appendKeys(vector<float> newKeys)
{
    for (float newKey : newKeys)
    {
        this->keys.push_back(newKey);
    }
}

void LeafNode::appendKeyAt(float key, int index)
{
    if (this->keys.size() >= this->numKeys)
    {
        this->keys.insert(this->keys.begin() + index, key);
    }
}

float LeafNode::getKey(int index)
{
    return this->keys.at(index);
}

vector<RecordPointer> LeafNode::getAllRecPtrs()
{
    return this->recPointer;
}

vector<Record *> LeafNode::getAllRecs()
{
    vector<RecordPointer> allRecPtr;
    vector<Record *> allRec;
    allRecPtr = this->recPointer;

    for (auto recPtr : allRecPtr)
    {
        Block *blk;
        int offset;
        Record *rec;

        // blk = recPtr.getBlock();
        // offset = recPtr.getOffset();
        // rec = blk->getRecords()[offset];
        rec = this->disk->getRecord(recPtr);

        allRec.push_back(rec);
    }

    return allRec;
}
