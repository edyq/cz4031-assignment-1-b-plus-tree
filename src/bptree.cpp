#include "bptree.h"

#include <math.h>
#include "cmath"
#include <algorithm>
#include <memory>
#include <vector>
#include "block.h"
#include "cassert"
#include "iostream"

using namespace std;

Node::Node(int maxNumKeys, Node *pPtr, bool isleaf) {
    parentPtr = pPtr;
    keys = *new vector<uint32_t>;
    numKeys = 0;
    maxKeys = maxNumKeys;
    isLeaf = isleaf;
    pointers = vector<Node *>();
    blocks = vector<vector<shared_ptr<Block>>>(1);
    nextNode = nullptr;
    preNode = nullptr;
}

BPTree::BPTree(int maxNumKeys) {
    root = nullptr;
    maxKeys = maxNumKeys;
    levels = 0;
    numNodes = 0;
}

void BPTree::remove(uint32_t key) { removeInternal(key); }

void BPTree::removeInternal(uint32_t key) {
    Node *cur_node = root;
    vector<uint32_t> keys = cur_node->getKeys();
    while (!cur_node->isLeafNode()) {
        bool get_node = false;
        for (size_t i = 0; i < keys.size(); i++) {
            if (key < keys[i]) {
                cur_node = cur_node->getChildNodes()[i];
                get_node = true;
                break;
            }
        }
        if (!get_node) {
            cur_node =
                    cur_node->getChildNodes()[cur_node->getChildNodes().size() - 1];
        }
        keys = cur_node->getKeys();
    }
    bool found = false;
    int index;
    keys = cur_node->getKeys();
    for (size_t i = 0; i < keys.size(); i++) {
        if (key == keys[i]) {
            found = true;
            index = i;
            break;
        }
    }
    cout << "found: " << found << endl;
    if (!found) {
        cout << "removing not found" << endl;
        return;
    } else {
        // call storage to remove that key using block
        cur_node->keys.erase(cur_node->keys.begin() + index);
        cur_node->blocks.erase(cur_node->blocks.begin() + index);
        cur_node->numKeys--;
        cout << "removed" << endl;
        while (cur_node != root && \
            ((cur_node->isLeafNode() && cur_node->numKeys < floor((float(cur_node->maxKeys) + 1) / 2)) || \
            (!cur_node->isLeafNode() && cur_node->numKeys < floor(float(cur_node->maxKeys) / 2)))
                ) {
            Node *preNode = cur_node->getPreNode();
            int mini_for_borrow;
            if (cur_node->isLeafNode()) {
                mini_for_borrow = floor((float(cur_node->maxKeys) + 1) / 2) + 1;
            } else {
                mini_for_borrow = floor(float(cur_node->maxKeys) / 2) + 1;
            }
            if (preNode &&
                preNode->numKeys >= mini_for_borrow) {
                // borrow from preNode
                cur_node->keys.insert(cur_node->keys.begin(),
                                      preNode->keys[preNode->keys.size() - 1]);
                preNode->keys.pop_back();
                if (cur_node->isLeafNode()) {
                    cur_node->blocks.insert(
                            cur_node->blocks.begin(),
                            preNode->blocks[preNode->blocks.size() - 1]);
                    preNode->blocks.pop_back();
                } else {
                    cur_node->pointers.insert(
                            cur_node->pointers.begin(),
                            preNode->pointers[preNode->pointers.size() - 1]);
                    preNode->pointers.pop_back();
                }

                cur_node->numKeys++;
                preNode->numKeys--;
                for (size_t i = 0; i < cur_node->parentPtr->keys.size(); i++) {
                    if (cur_node->parentPtr->keys[i] == cur_node->keys[1]) {
                        cur_node->parentPtr->keys[i] = cur_node->keys[0];
                    }
                }
                break;
            }
            Node *nextNode = cur_node->getNextNode();
            if (nextNode &&
                nextNode->numKeys >= mini_for_borrow) {
                // borrow from nextNode
                cur_node->keys.push_back(nextNode->keys[0]);
                nextNode->keys.erase(next(nextNode->keys.begin()));
                if (cur_node->isLeafNode()) {
                    cur_node->blocks.push_back(nextNode->blocks[0]);
                    nextNode->blocks.erase(next(nextNode->blocks.begin()));
                } else {
                    cur_node->pointers.push_back(nextNode->pointers[0]);
                    nextNode->pointers.erase(next(nextNode->pointers.begin()));
                }

                cur_node->numKeys++;
                cur_node->nextNode->numKeys--;
                break;
            }
            // cannot borrow, we need to do merge, and update recursively
            if (preNode && preNode->parentPtr != cur_node->parentPtr) {
                preNode = nullptr;
            }

            if (preNode &&
                cur_node->numKeys + preNode->numKeys <= cur_node->maxKeys) {
                cur_node = preNode;
            }
            nextNode = cur_node->getNextNode();
            if (nextNode->parentPtr != cur_node->parentPtr) {
                nextNode = nullptr;
            }
            cur_node->numKeys += nextNode->numKeys;
            move(nextNode->keys.begin(), nextNode->keys.end(),
                 std::back_inserter(cur_node->keys));

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
            for (size_t i = 0; i < parentPtr->pointers.size(); i++) {
                if (parentPtr->pointers[i] == cur_node) {
                    cur_index = i;
                    break;
                }
            }
            parentPtr->keys.erase(parentPtr->keys.begin() + cur_index);
            parentPtr->pointers.erase(parentPtr->pointers.begin() + cur_index +
                                      1);
            parentPtr->numKeys--;
            numNodes--;
            if (parentPtr->pointers.size() == 1) {
                parentPtr->isLeaf = cur_node->isLeaf;
                parentPtr->pointers = cur_node->pointers;
                parentPtr->blocks = cur_node->blocks;
                parentPtr->keys = cur_node->keys;
                parentPtr->numKeys = cur_node->numKeys;
            }
            if (root == parentPtr) {
                levels--;
            }
            cur_node = parentPtr;
        }
    }

    return;
}

SearchResult BPTree::search(uint32_t lbKey, uint32_t ubKey) {
    // single key search
    SearchResult result;
    Node *cursor = root;
    result.accessedNodes.push_back(root);

    // go to leaf node
    while (!cursor->isLeaf) {
        for (int i = 0; i < cursor->numKeys; i++) {

            if (lbKey < cursor->keys[i]) {
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
    // search the level of leaf nodes
    while (true) {
        for (int i = 0; i < cursor->numKeys; i++) {
            if (cursor->keys[i] >= lbKey && cursor->keys[i] <= ubKey) {
                for (size_t j = 0; j < cursor->blocks[i].size(); j++) {
                    result.accessedBlocks.insert(cursor->blocks[i][j]);
                }
            }
        }
        if (cursor->nextNode != nullptr) {
            cursor = cursor->nextNode;
        } else break;
        if (cursor == nullptr) break;
        if (cursor->keys[0] > ubKey) break;
        result.accessedNodes.push_back(cursor);
    }

    return result;
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
        numNodes++;
    } else {
        // else the tree exists, traverse the tree to find the proper place to
        // insert the key
        Node *cursor = root;  // start from the root

        // while not leaf, keep traversing
        while (!cursor->isLeaf) {
            for (int i = 0; i < cursor->numKeys; i++) {
                // if the new key is less than the current key, go to left
                // pointer's node
                if (key < cursor->keys[i]) {
                    Node *leftNode = cursor->pointers[i];
                    cursor = leftNode;
                    break;
                }
                // reaching last key in the node
                if (i == cursor->numKeys - 1) {
                    Node *endNode = cursor->pointers.back();
                    cursor = endNode;
                    break;
                }
            }
        }

        // at leaf node now;
        uint32_t keyIndex =
                find(cursor->keys.begin(), cursor->keys.end(), key) -
                cursor->keys.begin();
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
                vector<vector<shared_ptr<Block>>> tempBlockVector =
                        cursor->blocks;

                int i = 0;
                while (i < maxKeys && key > tempKeyVector[i]) i++;

                tempKeyVector.insert(tempKeyVector.begin() + i, key);
                vector<shared_ptr<Block>> newVector = {blockAddress};
                tempBlockVector.insert(tempBlockVector.begin() + i, newVector);

                // Split the two new nodes into two.
                // ⌊(n+1)/2⌋ keys for left, n+1 - ⌊(n+1)/2⌋ (aka remaining) keys
                // for right.
                cursor->numKeys = (maxKeys + 1) / 2;
                newLeafNode->numKeys = (maxKeys + 1) - ((maxKeys + 1) / 2);

                cursor->keys =
                        vector<uint32_t>(tempKeyVector.begin(),
                                         tempKeyVector.begin() + cursor->numKeys);
                newLeafNode->keys =
                        vector<uint32_t>(tempKeyVector.begin() + cursor->numKeys,
                                         tempKeyVector.end());

                cursor->blocks = vector<vector<shared_ptr<Block>>>(
                        tempBlockVector.begin(),
                        tempBlockVector.begin() + cursor->numKeys);
                newLeafNode->blocks = vector<vector<shared_ptr<Block>>>(
                        tempBlockVector.begin() + cursor->numKeys,
                        tempBlockVector.end());

                if (cursor->nextNode != nullptr) {
                    cursor->nextNode->preNode = newLeafNode;
                }
                newLeafNode->nextNode = cursor->nextNode;
                cursor->nextNode = newLeafNode;
                newLeafNode->preNode = cursor;

                numNodes++;

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

                    numNodes++;
                    levels++;
                } else {
                    // update the internal parent
                    updateInternalParent(newLeafNode->keys[0], parentNode,
                                         newLeafNode);
                }
            }
        }
    }
}

void BPTree::updateInternalParent(uint32_t key, Node *cursor,
                                  Node *newLeafNode) {
    // if parent still have space, just insert the 2 new nodes
    if (cursor->numKeys < maxKeys) {
        // update leaf nodes' parentPtr
        newLeafNode->parentPtr = cursor;

        // update keys
        int i = 0;
        while (key > cursor->keys[i] && i < cursor->numKeys) i++;
        cursor->keys.insert(cursor->keys.begin() + i, key);

        // update pointers
        cursor->pointers.insert(cursor->pointers.begin() + i + 1, newLeafNode);

        // update numKeys
        cursor->numKeys += 1;
        for (auto node: cursor->pointers) {
            node->parentPtr = cursor;
        }
    } else {
        // the parent no longer has space; need to split the parent also, and
        // continue checking parent of the parent
        Node *newInternalNode = new Node(maxKeys, nullptr, false);
        Node *parentPtr = cursor->parentPtr;

        vector<uint32_t> tempKeyVector = cursor->keys;
        vector<Node *> tempPtrVector = cursor->pointers;

        int i = 0;
        while (key > cursor->keys[i] && i < cursor->numKeys) i++;

        tempKeyVector.insert(tempKeyVector.begin() + i, key);
        tempPtrVector.insert(tempPtrVector.begin() + i + 1, newLeafNode);

        auto leftNumOfPtrs = std::ceil((float) maxKeys / 2) + 1;

        auto leftPtrs = vector<Node *>(tempPtrVector.begin(),
                                       tempPtrVector.begin() + leftNumOfPtrs);
        assert(leftPtrs.size() > 0);
        auto rightPtrs = vector<Node *>(tempPtrVector.begin() + leftNumOfPtrs,
                                        tempPtrVector.end());
        assert(rightPtrs.size() > 0);

        vector<uint32_t> leftKeys;
        Node *travel;
        for (size_t j = 0; j < leftPtrs.size() - 1; j++) {
            travel = leftPtrs[j + 1];
            while (!travel->isLeaf) {
                travel = travel->pointers[0];
            }
            leftKeys.emplace_back(travel->getKeys()[0]);
        }

        vector<uint32_t> rightKeys;
        for (size_t j = 0; j < rightPtrs.size() - 1; j++) {
            travel = rightPtrs[j + 1];
            while (!travel->isLeaf) {
                travel = travel->pointers[0];
            }
            rightKeys.emplace_back(travel->getKeys()[0]);
        }

        cursor->numKeys = std::ceil((float) maxKeys / 2);
        newInternalNode->numKeys = std::floor((float) maxKeys / 2);

        cursor->keys = leftKeys;
        newInternalNode->keys = rightKeys;

        cursor->pointers = vector<Node *>(
                tempPtrVector.begin(), tempPtrVector.begin() + cursor->numKeys + 1);
        newInternalNode->pointers = vector<Node *>(
                tempPtrVector.begin() + cursor->numKeys + 1, tempPtrVector.end());

        newInternalNode->parentPtr = cursor->parentPtr;

        numNodes++;

        // update the parent
        if (cursor == root) {
            // if the current node is root itself, we need a new root
            Node *newRoot = new Node(maxKeys, nullptr, false);

            travel = newInternalNode->pointers[0];
            while (!travel->isLeaf) {
                travel = travel->pointers[0];
            }
            newRoot->keys.push_back(travel->getKeys()[0]);

            newRoot->pointers.push_back(cursor);
            newRoot->pointers.push_back(newInternalNode);
            newRoot->numKeys += 1;
            root = newRoot;

            cursor->parentPtr = root;
            newInternalNode->parentPtr = root;

            numNodes++;
            levels++;
            for (auto node: newInternalNode->pointers) {
                node->parentPtr = newInternalNode;
            }
            for (auto node: cursor->pointers) {
                node->parentPtr = cursor;
            }
        } else {
            for (auto node: newInternalNode->pointers) {
                node->parentPtr = newInternalNode;
            }
            for (auto node: cursor->pointers) {
                node->parentPtr = cursor;
            }
            travel = newInternalNode->pointers[0];
            while (!travel->isLeaf) {
                travel = travel->pointers[0];
            }
            // parent is internal,
            updateInternalParent(travel->getKeys()[0], cursor->parentPtr,
                                 newInternalNode);
        }
    }
}
