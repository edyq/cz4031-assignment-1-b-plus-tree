#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
#include <algorithm>

using namespace std;

Node::Node(int maxNumKeys, Node *parentPtr, bool isleaf) {
    parentPtr = parentPtr;
    keys = *new vector<uint32_t>;
    numKeys = 0;
    maxKeys = maxNumKeys;
    isLeaf = isleaf;
    pointers = *new vector<Node *>;
    blocks = *new vector<vector<shared_ptr<Block>>>;
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
    while (!cur_node->isLeafNode()) {
        for (int i = 0; i < keys.size(); i++) {
            if (key < keys[i]) {
                cur_node = cur_node->getChildNodes()[i];
            }
        }
    }
    bool found = false;
    int index;
    for (int i = 0; i < keys.size(); i++) {
        if (key == keys[i]) {
            vector<shared_ptr<Block>> block = cur_node->getBlocks()[i];
            found = true;
            index = i;
            break;
        }
    }

    if (!found) {
        return;
    } else {
        // call storage to remove that key using block
        cur_node->keys.erase(cur_node->keys.begin() + index);
        cur_node->blocks.erase(cur_node->blocks.begin() + index);
        cur_node->numKeys--;
//        for parent node deletion, we consider the case
        while (cur_node->numKeys < cur_node->maxKeys / 2) {
            // borrow from preNode
            if (cur_node->preNode->numKeys >= cur_node->preNode->maxKeys / 2 + 1) {
                cur_node->keys.insert(cur_node->keys.begin(),
                                      cur_node->preNode->keys[cur_node->preNode->keys.size() - 1]);
                cur_node->preNode->keys.pop_back();
                cur_node->pointers.insert(cur_node->pointers.begin(),
                                          cur_node->preNode->pointers[cur_node->preNode->pointers.size() - 1]);
                cur_node->preNode->pointers.pop_back();
                cur_node->numKeys++;
                cur_node->preNode->numKeys--;
                for (int i = 0; i < cur_node->parentPtr->keys.size(); i++) {
                    if (cur_node->parentPtr->keys[i] == cur_node->keys[1]) {
                        cur_node->parentPtr->keys[i] = cur_node->keys[0];
                    }
                }
                break;
            }
            if (cur_node->nextNode->numKeys >= cur_node->nextNode->maxKeys / 2 + 1) {
                // borrow from nextNode
                cur_node->keys.push_back(cur_node->nextNode->keys[0]);
                cur_node->nextNode->keys.erase(next(cur_node->nextNode->keys.begin()));
                cur_node->pointers.push_back(cur_node->nextNode->pointers[0]);
                cur_node->nextNode->pointers.erase(next(cur_node->nextNode->pointers.begin()));
                cur_node->numKeys++;
                cur_node->nextNode->numKeys--;

                break;
            }
            // cannot borrow, we need to do merge, and update recursively

            if (cur_node->preNode && cur_node->numKeys + cur_node->preNode->numKeys <= cur_node->maxKeys) {
                cur_node = cur_node->preNode;
            }


            cur_node->numKeys += cur_node->nextNode->numKeys;
            move(cur_node->nextNode->keys.begin(), cur_node->nextNode->keys.end(), std::back_inserter(cur_node->keys));

            if (cur_node->isLeafNode()) {
                move(cur_node->nextNode->blocks.begin(), cur_node->nextNode->blocks.end(),
                     std::back_inserter(cur_node->blocks));
            } else {
                move(cur_node->nextNode->pointers.begin(), cur_node->nextNode->pointers.end(),
                     std::back_inserter(cur_node->pointers));
            }
            cur_node->nextNode = cur_node->nextNode->nextNode;
            cur_node = cur_node->parentPtr;
        }
    }

    return;
}

vector<SearchResult> BPTree::search(uint32_t lbKey, uint32_t ubKey) {
    // single key search
    vector<SearchResult> result = vector<SearchResult>();
    if (lbKey == ubKey) {
        result.push_back(searchKey(lbKey));
    } else {
        // range-based search
        for (uint32_t key = lbKey; key <= ubKey; key++) {
            result.push_back(searchKey(key));
        }
    }
    return result;
}

SearchResult BPTree::searchKey(uint32_t key) {
    Node *cursor = root;
    SearchResult result = SearchResult();
    result.accessedNodes.push_back(root);
    // search to leaf node containing the key
    while (!cursor->isLeaf) {
        for (int i = 0; i < cursor->numKeys; i++) {
            if (key < cursor->keys[i]) {
                cursor = cursor->pointers[i];
                result.accessedNodes.push_back(cursor);
                break;
            }
            if (i == cursor->numKeys - 1) {
                cursor = cursor->pointers.back();
                result.accessedNodes.push_back(cursor);
                break;
            }
        }
    }
    // search for the key at this leaf node
    for (int i = 0; i < cursor->numKeys; i++) {
        if (cursor->keys[i] == key) {
            result.accessedBlocks.insert(result.accessedBlocks.end(), cursor->blocks[i].begin(),
                                         cursor->blocks[i].end());
        }
    }
    return result;
};