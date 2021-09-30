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
void BPTree::removeInternal(uint32_t key) {
    Node *cur_node = root;
    vector<uint32_t> keys = cur_node->keys;
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
            block = cur_node->getBlocks()[i];
            found = true;
            index = i;
            break;
        }
    }
    if (!found){
        print("key not found, cannot remove")
        return;
    }else{
        // call storage to remove that key
        cur_node->keys.erase( cur_node->keys.begin() + index );
        cur_node->blocks.erase( cur_node->blocks.begin() + index );
        cur_node->numKeys --;

        while(cur_node->numKeys+cur_node->preNode->numKeys <= cur_node->maxKeys || cur_node->numKeys+cur_node->nextNode->numKeys <= cur_node->maxKeys){
            if (cur_node->numKeys + cur_node->preNode->numKeys <= cur_node->maxKeys){
                cur_node = cur_node->preNode;
            }
            vector<Node *> nodeList = cur_node->getParentPtr()->getChildNodes();
            int index = distance(nodeList.begin(), cur_node);
            cur_node->numKeys += cur_node->nextNode->numKeys;
            move(cur_node->nextNode->keys.begin(), cur_node->nextNode->keys.end(), std::back_inserter(cur_node->keys));

            if (cur_node->isLeafNode()){
                move(cur_node->nextNode->blocks.begin(), cur_node->nextNode->blocks.end(), std::back_inserter(cur_node->blocks));
            }else{
                move(cur_node->nextNode->pointers.begin(), cur_node->nextNode->pointers.end(), std::back_inserter(cur_node->pointers));
            }
            cur_node->nextNode = cur_node->nextNode->nextNode;
            cur_node = cur_node->parentPtr;
            }
        }
    }
    return block;
}
