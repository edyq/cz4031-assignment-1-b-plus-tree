#include "bptree.h"
#include "block.h"
#include "iostream"
#include <vector>
using namespace std;

InternalNode::InternalNode(int maxNumKeys, Node *parentPtr) {
    Node* parentPtr = parentPtr;
    vector<uint32_t>* keys = new vector<uint32_t>;
    int numKeys = 0;
    int maxKeys = maxNumKeys;
    bool isLeaf = false;
    vector<Node *>* pointers = new vector<Node *>;

}

LeafNode::LeafNode(int maxNumKeys, Node *parentPtr) {
    Node* parentPtr = parentPtr;
    vector<uint32_t>* keys = new vector<uint32_t>;
    int numKeys = 0;
    int maxKeys = maxNumKeys;
    bool isLeaf = true;
    vector<shared_ptr<Block>>* blocks = new vector<shared_ptr<Block>>;
    Node* nextNode = nullptr;
}
