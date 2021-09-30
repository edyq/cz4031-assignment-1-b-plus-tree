#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
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
}

BPTree::BPTree(int maxNumKeys) {
    root = nullptr;
    maxKeys = maxNumKeys;
    levels = 0;
    numNodes = 0;
}

// insert a record into the bptree
void BPTree::insertInternal(uint32_t key, shared_ptr<Block> blockAddress) {
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
        // TODO: we should check whether the new key is contained in the key list;

        int keyIndex = find(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.end();
        if (keyIndex < cursor->keys.size()) {
            // no new key needs to be inserted
            cursor->blocks[keyIndex].push_back(blockAddress);
        } else {
            // new key has to be inserted;
            if (cursor->numKeys < maxKeys) {
                // there is place for a new key in the current node
                int i = 0;
                while (i < cursor->numKeys and key > cursor->keys[i]) i++;

                cursor->keys.insert(cursor->keys.begin() + i, key);
                cursor->numKeys += 1;
                vector<shared_ptr<Block>> newVector = {blockAddress};
                cursor->blocks.insert(cursor->keys.begin() + i, newVector);
            } else {
                // current node is full, split the node, update the parent, recursively T.T
                Node *parentNode = cursor->parentPtr;
                Node *newLeafNode = Node(maxKeys, nullptr, true);

                vector<uint32_t> tempKeyList = cursor->keys;
                vector<vector<shared_ptr<Block>>> tempBlockVector = cursor->blocks;
                Node *nextNode = cursor->pointers[-1];      // leaf nodes have only one pointer to other nodes

                int i = 0;
                while (i < maxKeys and key > tempKeyList[i]) i++;
                tempKeyList.insert(tempKeyList.begin() + i, key);
                vector<shared_ptr<Block>> newVector = {blockAddress};
                tempBlockVector.insert(tempBlockVector.begin() + i, newVector);


            }
        }
    }
}
