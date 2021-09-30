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
        root->blocks.push_back(blockAddress);
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

        // at leaf node; insert key here
        if (cursor->numKeys < maxKeys) {
            int i = 0;
            while (i < cursor->numKeys) {
                if (key < cursor->keys[i]) break;
                i++;
            }
            cursor->keys.insert(cursor->keys.begin() + i, key);

            cursor->keys.push_back(key);
            cursor->numKeys += 1;
            cursor->blocks.push_back(blockAddress);
        }
    }
}
