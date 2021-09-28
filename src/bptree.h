#ifndef BP_TREE_H
#define BP_TREE_H

#define ORDER_V 5
#define MAXNUM_KEY (ORDER_V * 2)
#define MAXNUM_POINTER (MAXNUM_KEY + 1)
#define MAXNUM_DATA (ORDER_V * 2)

typedef int KEY_TYPE;

class BPNode{
private:
    Address *pointers;
    float *keys;
    int num_keys;
    int max_keys;
    bool isLeaf;
    friend class BPTree;
public:
    BPNode(int maxKeys);
};

class BPTree{
private:
    BPNode *root;
    int maxKeys;
    int levels;
    int numNodes;
    void insInternal(float key);
    void rmInternal(float key);

public:
    BPlusTree(int maxKeys);
    void search(float lbKey, float ubKey);
    void insert(float key);
    int remove(float key);


};


#endif