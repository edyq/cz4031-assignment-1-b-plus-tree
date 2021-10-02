#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
#include <algorithm>
using namespace std;


Node::Node(int maxNumKeys, Node *parentPointer, bool isLeafNode) {
    parentPtr = parentPointer;
    keys = vector<uint32_t>();
    numKeys = 0;
    maxKeys = maxNumKeys;
    isLeaf = isLeafNode;
    pointers = vector<Node *>();
    blocks = vector<vector<shared_ptr<Block>>>(maxKeys);
    nextNode = nullptr;
    preNode = nullptr;
}

BPTree::BPTree(int maxNumKeys) {
    root = nullptr;
    maxKeys = maxNumKeys;
    levels = 0;
    numNodes = 0;
}

void BPTree::remove(uint32_t key) {
    removeInternal(key);
}

void BPTree::removeInternal(uint32_t key) {
    Node *cur_node = root;
    vector<uint32_t> keys = cur_node->getKeys();
    while(!cur_node->isLeafNode()){
        for (int i=0; i<keys.size(); i++){
            if (key < keys[i]){
                cur_node = cur_node->getChildNodes()[i];
            }
        }
    }
    bool found = false;
    int index;
    for(int i=0; i<keys.size(); i++){
        if (key == keys[i]) {
            vector<shared_ptr<Block>> block = cur_node->getBlocks()[i];
            found = true;
            index = i;
            break;
        }
    }

    if (!found){
        return;
    }else {
        // call storage to remove that key using block
        cur_node->keys.erase(cur_node->keys.begin() + index);
        cur_node->blocks.erase(cur_node->blocks.begin() + index);
        cur_node->numKeys--;
//        for parent node deletion, we consider the case
        while (cur_node->numKeys < cur_node->maxKeys/2) {

            Node *preNode = cur_node->getPreNode();
            if (preNode->numKeys >= preNode->maxKeys/2+1){
                // borrow from preNode
                cur_node->keys.insert(cur_node->keys.begin(), preNode->keys[preNode->keys.size()-1]);
                preNode->keys.pop_back();
                cur_node->pointers.insert(cur_node->pointers.begin(), preNode->pointers[preNode->pointers.size()-1]);
                preNode->pointers.pop_back();
                cur_node->numKeys ++;
                preNode->numKeys --;
                for (int i=0; i<cur_node->parentPtr->keys.size(); i++){
                    if (cur_node->parentPtr->keys[i] == cur_node->keys[1]){
                        cur_node->parentPtr->keys[i] = cur_node->keys[0];
                    }
                }
                break;
            }
            Node *nextNode = cur_node->getNextNode();
            if (nextNode->numKeys >= nextNode->maxKeys/2+1){
                // borrow from nextNode

                cur_node->keys.push_back(nextNode->keys[0]);
                nextNode->keys.erase(next(nextNode->keys.begin()));
                cur_node->pointers.push_back(nextNode->pointers[0]);
                nextNode->pointers.erase(next(nextNode->pointers.begin()));
                cur_node->numKeys ++;
                cur_node->nextNode->numKeys --;
                break;
            }
            // cannot borrow, we need to do merge, and update recursively
//            Node *preNode = cur_node->getPreNode();
            if (preNode && cur_node->numKeys + preNode->numKeys <= cur_node->maxKeys) {
                cur_node = preNode;
            }

//            Node *nextNode = cur_node->getNextNode();

            cur_node->numKeys += nextNode->numKeys;
            move(nextNode->keys.begin(), nextNode->keys.end(), std::back_inserter(cur_node->keys));

            if (cur_node->isLeafNode()) {
                move(nextNode->blocks.begin(), nextNode->blocks.end(),
                     std::back_inserter(cur_node->blocks));
                cur_node->nextNode = cur_node->nextNode->nextNode;
            } else {
                move(nextNode->pointers.begin(), nextNode->pointers.end(),
                     std::back_inserter(cur_node->pointers));
            }


            Node *parentPtr = cur_node->parentPtr;
            int cur_index = -1;
            for (int i=0; i<parentPtr->pointers.size(); i++){
                if (parentPtr->pointers[i] == cur_node){
                    cur_index = i;
                }
            }
            parentPtr->keys.erase(parentPtr->keys.begin() + cur_index+1);
            parentPtr->pointers.erase(parentPtr->pointers.begin() + cur_index+1);
            parentPtr->numKeys--;
            numNodes--;
            if(parentPtr->pointers.size()==1){
                parentPtr->isLeaf = cur_node->isLeaf;
                parentPtr->pointers = cur_node->pointers;
                parentPtr->blocks = cur_node->blocks;
                parentPtr->keys = cur_node->keys;
                parentPtr->numKeys = cur_node->numKeys;
            }
            if (root == parentPtr){
                levels--;
            }

            cur_node = cur_node->parentPtr;
        }
    }

    return ;
}

// insert a record into the bptree
void BPTree::insert(uint32_t key, shared_ptr<Block> blockAddress) {
    // if there is no root, create a new bptree root node
    if (root == nullptr) {
        root = new Node(maxKeys, nullptr, true);
        root->keys.push_back(key);
        root->numKeys += 1;
        root->blocks[0].push_back(blockAddress);
        levels = 1;
    } else {
        // else the tree exists, traverse the tree to find the proper place to insert the key
        Node *cursor = root;        // start from the root

        // while not leaf, keep traversing
        while (!cursor->isLeaf) {
            for (int i = 0; i < cursor->numKeys; i++) {
                // if the new key is less than the current key, go to left pointer's node
                if (key < cursor->keys[i]) {
                    Node *leftNode = cursor->pointers[i];
                    cursor = leftNode;
                    break;
                }
                // reaching last key in the node
                if (i == cursor->numKeys - 1) {
                    // todo: change -1; pointers
                    Node *endNode = cursor->pointers.back();
                    cursor = endNode;
                    break;
                }
            }
        }

        // at leaf node now;
        uint32_t keyIndex = find(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();
        if (keyIndex < cursor->keys.size()) {
            // no new key needs to be inserted
            cursor->blocks[keyIndex].push_back(blockAddress);
        } else {
            // new key has to be inserted;
            if (cursor->numKeys < maxKeys) {
                // there is place for a new key in the current node
                // no update required for prev / next pointers
                int i = 0;
                while (i < cursor->numKeys && key > cursor->keys[i]) i++;

                cursor->keys.insert(cursor->keys.begin() + i, key);
                cursor->numKeys += 1;
                vector<shared_ptr<Block>> newVector = {blockAddress};
                cursor->blocks.insert(cursor->blocks.begin() + i, newVector);
            } else {
                // current node is full, split the node, update the parent
                // update prev / next pointers
                Node *parentNode = cursor->parentPtr;
                Node *newLeafNode = new Node(maxKeys, nullptr, true);

                vector<uint32_t> tempKeyVector = cursor->keys;
                vector<vector<shared_ptr<Block>>> tempBlockVector = cursor->blocks;

                int i = 0;
                while (i < maxKeys && key > tempKeyVector[i]) i++;

                tempKeyVector.insert(tempKeyVector.begin() + i, key);
                vector<shared_ptr<Block>> newVector = {blockAddress};
                tempBlockVector.insert(tempBlockVector.begin() + i, newVector);

                // Split the two new nodes into two.
                // ⌊(n+1)/2⌋ keys for left, n+1 - ⌊(n+1)/2⌋ (aka remaining) keys for right.
                cursor->numKeys = (maxKeys + 1) / 2;
                newLeafNode->numKeys = (maxKeys + 1) - ((maxKeys + 1) / 2);

                cursor->keys = vector<uint32_t>(tempKeyVector.begin(), tempKeyVector.begin() + cursor->numKeys - 1);
                newLeafNode->keys = vector<uint32_t>(tempKeyVector.begin() + cursor->numKeys, tempKeyVector.end());

                cursor->blocks = vector<vector<shared_ptr<Block>>>(tempBlockVector.begin(), tempBlockVector.begin() + cursor->numKeys - 1);
                newLeafNode->blocks = vector<vector<shared_ptr<Block>>>(tempBlockVector.begin() + cursor->numKeys, tempBlockVector.end());

                cursor->nextNode = newLeafNode;
                newLeafNode->preNode = cursor;

                numNodes ++;

                // update the parent
                if (cursor == root) {
                    // if the current node is root itself, we need a new root
                    Node *newRoot = new Node(maxKeys, nullptr, false);

                    newRoot->keys.push_back(newLeafNode->keys[0]);
                    newRoot->pointers.push_back(cursor);
                    newRoot->pointers.push_back(newLeafNode);
                    newRoot->numKeys += 1;
                    root = newRoot;
                    cursor->parentPtr = root;
                    newLeafNode->parentPtr = root;

                    numNodes += 2;
                    levels ++;
                } else {
                    // update the internal parent
                    updateInternalParent(newLeafNode->keys[0], parentNode, newLeafNode);
                }
            }
        }
    }
}

void BPTree::updateInternalParent(uint32_t key, Node *cursor, Node *newLeafNode) {
    // if parent still have space, just insert the 2 new nodes
    if (cursor->numKeys < maxKeys) {
        // update leaf nodes' parentPtr
        newLeafNode->parentPtr = cursor;

        // update keys
        int i = 0;
        while (key > cursor->keys[i] && i < cursor->numKeys) i++;
        cursor->keys.insert(cursor->keys.begin() + i, key);

        // update pointers
        cursor->pointers.insert(cursor->pointers.begin() + i, newLeafNode);

        // update numKeys
        cursor->numKeys += 1;
    } else {
        // the parent no longer has space; need to split the parent also, and continue checking parent of the parent
        Node *newInternalNode = new Node(maxKeys, nullptr, false);
        Node *parentPtr = cursor->parentPtr;

        vector<uint32_t> tempKeyVector = cursor->keys;
        vector<Node *> tempPtrVector = cursor->pointers;

        int i = 0;
        while (key > cursor->keys[i] && i < cursor->numKeys) i++;

        tempKeyVector.insert(tempKeyVector.begin() + i, key);
        tempPtrVector.insert(tempPtrVector.begin() + i, newLeafNode);

        cursor->numKeys = (maxKeys + 1) / 2;
        newInternalNode->numKeys = maxKeys - (maxKeys + 1) / 2;

        cursor->keys = vector<uint32_t>(tempKeyVector.begin(), tempKeyVector.begin() + cursor->numKeys - 1);
        newInternalNode->keys = vector<uint32_t>(tempKeyVector.begin() + cursor->numKeys, tempKeyVector.end());

        cursor->pointers = vector<Node *>(tempPtrVector.begin(), tempPtrVector.begin() + cursor->numKeys - 1);
        newInternalNode->pointers = vector<Node *>(tempPtrVector.begin() + cursor->numKeys, tempPtrVector.end());

        newInternalNode->parentPtr = cursor->parentPtr;

        numNodes ++;

        // update the parent
        if (cursor == root) {
            // if the current node is root itself, we need a new root
            Node *newRoot = new Node(maxKeys, nullptr, false);

            newRoot->keys.push_back(newInternalNode->keys[0]);
            newRoot->pointers.push_back(cursor);
            newRoot->pointers.push_back(newInternalNode);
            newRoot->numKeys += 1;
            root = newRoot;

            cursor->parentPtr = root;
            newInternalNode->parentPtr = root;

            numNodes += 2;
            levels ++;
        } else {
            // parent is internal,
            updateInternalParent(newInternalNode->keys[0], cursor->parentPtr, newInternalNode);
        }
    }
}

SearchResult BPTree::search(uint32_t lbKey, uint32_t ubKey) {
	return SearchResult();
}
