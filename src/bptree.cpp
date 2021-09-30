#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
#include <algorithm>
using namespace std;

Node::Node(int maxNumKeys, Node *parentPtr, bool isLeaf) {
    parentPtr = parentPtr;
    keys = * new vector<uint32_t>;
    numKeys = 0;
    maxKeys = maxNumKeys;
    isLeaf = isLeaf;
    pointers = * new vector<Node *>;
    blocks = * new vector<vector<shared_ptr<Block>>>;
    nextNode = nullptr;
    preNode = nullptr;
}

BPTree::BPTree(int maxNumKeys) {
    root = nullptr;
    maxKeys = maxNumKeys;
    levels = 0;
    numNodes = 0;
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
                    Node *endNode = cursor->pointers[-1];
                    cursor = endNode;
                    break;
                }
            }
        }

        // at leaf node now;
        int keyIndex = int(find(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.end());
        if (keyIndex < cursor->keys.size()) {
            // no new key needs to be inserted
            cursor->blocks[keyIndex].push_back(blockAddress);
        } else {
            // new key has to be inserted;
            if (cursor->numKeys < maxKeys) {
                // there is place for a new key in the current node
                // no update required for prev / next pointers
                int i = 0;
                while (i < cursor->numKeys and key > cursor->keys[i]) i++;

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
                Node *nextNode = cursor->pointers[-1];      // leaf nodes have only one pointer to other nodes

                int i = 0;
                while (i < maxKeys and key > tempKeyVector[i]) i++;
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

                if (cursor == root) {
                    Node *newRoot = new Node(maxKeys, nullptr, false);

                    newRoot->keys.push_back(newLeafNode->keys[0]);
                    newRoot->pointers.push_back(cursor);
                    newRoot->pointers.push_back(newLeafNode);
                    newRoot->numKeys += 1;
                    root = newRoot;
                } else {
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
        // the parent no longer has space; do recursion here
        Node *newInternalNode = new Node(maxKeys, nullptr, false);

    }
}