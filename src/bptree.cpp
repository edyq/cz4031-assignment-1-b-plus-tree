#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
#include <algorithm>
using namespace std;

Node::Node(int maxNumKeys, Node *parentPtr, bool isleaf) {
    parentPtr = parentPtr;
    keys = * new vector<uint32_t>;
    numKeys = 0;
    maxKeys = maxNumKeys;
    isLeaf = isleaf;
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

vector<SearchResult> BPTree::search(uint32_t lbKey, uint32_t ubKey) {}
