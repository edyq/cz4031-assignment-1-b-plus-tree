#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include "block.h"

using namespace std;

struct SearchResult {
    vector<Node *> accessedNodes;
    vector<shared_ptr<Block>> accessedBlocks;
};

#endif