#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <set>
#include <queue>

#include "storage.h"
#include "utils.h"
#include "bptree.h"

std::vector<Entry> LoadEntryFromFileOrDie(
        const std::string filename = "../data/data.tsv") {
    std::ifstream fin(filename);
    assert(fin.good() && "failed to open file for data ");
    std::string infile;
    std::vector<Entry> contents;
    size_t counter = 0;
    Entry current_entry;
    while (fin >> infile) {
        if (++counter <= 3) continue;  // header, not needed
        switch (counter % 3) {
            case 1:  // tconst
                for (size_t i = 0; i < 10; i++) {
                    if (i < infile.size()) {
                        *(current_entry.tconst + i) = infile[i];
                    } else {
                        *(current_entry.tconst + i) = Entry::tconstTerminator;
                    }
                }
                break;
            case 2:  // averageRating
                current_entry.rating = (infile[0] - '0') * 10 + (infile[2] - '0');
                break;
            case 0:  // numVotes
                current_entry.numVotes = std::stoul(infile);
                contents.push_back(current_entry);  // no need to clear
                break;
        }
    }
    return contents;
}

/**
 * Output experiment result.
 * @param accessedNodes accessed index nodes during search
 */
void printIndexNodes(vector<Node *> &accessedNodes) {
    int numOfIndexNodes = accessedNodes.size();
    std::cout << "Number of index nodes accessed: " << numOfIndexNodes << std::endl;
    std::cout << "Contents of index nodes accessed (at most 5 are printed): " << std::endl;
    auto beg = accessedNodes.begin();
    for (int i = 0; i < 5 && beg != accessedNodes.end(); i++, beg++) {
        std::cout << "Node " << i + 1 << ":" << endl;
        printVector<uint32_t>((*beg)->getKeys());
    }
}

/**
 * Output experiment result.
 * @param accessedBlocks accessed blocks during search
 */
void printBlocks(set<shared_ptr<Block>> &accessedBlocks) {
    int numOfBlocks = accessedBlocks.size();
    std::cout << "Number of blocks accessed: " << numOfBlocks << std::endl;
    std::cout << "Contents of blocks accessed (at most 5 are printed): " << std::endl;
    auto beg = accessedBlocks.begin();
    for (int i = 0; i < 5 && beg != accessedBlocks.end(); i++, beg++) {
        Storage::print(*beg);
    }
}

void printAvg(Storage &storage, set<shared_ptr<Block>> &accessedBlocks, uint32_t lb, uint32_t ub) {
    // Search and calculate average
    double sum = 0;
    int count = 0;

    vector<Entry> allEntries;

    for (auto beg = accessedBlocks.begin(); beg != accessedBlocks.end(); beg++) {
        auto entries = storage.query(*beg, lb, ub);
        for (auto entry: entries) {
            sum += entry.getRating();
            allEntries.push_back(entry);
            count++;
        }
    }
    double avg = sum / count;
    std::cout << "average of averageRating = " << avg << std::endl;
}

void run_exp_3(Storage &storage, BPTree &indexTree) {
    std::cout << "=========================" << std::endl;
    std::cout << "Experiment 3:" << std::endl;
    /**
       * Experiment 3: retrieve those movies with the “numVotes” equal to 500 and report the following statistics:
        - the number and the content of index nodes the process accesses
        - the number and the content of data blocks the process accesses
        - the average of “averageRating’s” of the records that are returned
     */
    int lb = 500, ub = 500;
    auto result = indexTree.search(lb, ub);
    printIndexNodes(result.accessedNodes);
    printBlocks(result.accessedBlocks);
    printAvg(storage, result.accessedBlocks, lb, ub);
}

void run_exp_4(Storage &storage, BPTree &indexTree) {
    std::cout << "=========================" << std::endl;
    std::cout << "Experiment 4:" << std::endl;
    /**
     *  Experiment 4: retrieve those movies with the attribute “numVotes” from 30,000 to 40,000, both inclusively and report the following statistics:
        - the number and the content of index nodes the process accesses
        - the number and the content of data blocks the process accesses
        - the average of “averageRating’s” of the records that are returned
     */
    int lb = 30000, ub = 40000;
    auto result = indexTree.search(lb, ub);

    printIndexNodes(result.accessedNodes);
    printBlocks(result.accessedBlocks);
    printAvg(storage, result.accessedBlocks, lb, ub);
}

void run_experiments(Storage &storage, std::vector<Entry> &entries, BPTree &indexTree) {
    auto target_blocks = storage.initBatchInsertEntries(entries);
    assert (target_blocks.size() == entries.size() && "target_blocks size must be the same with entries");
    std::set<std::shared_ptr<Block>> used_blocks(target_blocks.begin(), target_blocks.end());
    for (size_t i = 0; i < target_blocks.size(); i++) {
        indexTree.insert(entries[i].numVotes, target_blocks[i]);
    }
    std::cout << "=========================" << std::endl;
    std::cout << "Experiment 1:" << std::endl;
    std::cout << "the number of blocks: " << used_blocks.size() << std::endl;
    std::cout << "the size of database (in terms of MB) " << bytesToMb(entries.size() * Entry::size) << std::endl;

    std::cout << "=========================" << std::endl;
    std::cout << "Experiment 2:" << std::endl;
    std::cout << "parameter n of the B+ tree: " << indexTree.getMaxKeys() << std::endl;
    std::cout << "the number of nodes of the B+ tree: " << indexTree.getNumNodes() << std::endl;
    std::cout << "the height of the B+ tree, i.e., the number of levels of the B+ tree: " << indexTree.getLevels()
              << std::endl;
    auto rootNode = indexTree.getRoot();
    std::cout << "the content of the root node: ";
    printVector<uint32_t>(rootNode->getKeys());

    // Experiment 3
    run_exp_3(storage, indexTree);


    // Experiment 4
    run_exp_4(storage, indexTree);


    // Experiment 5
    std::cout << "=========================" << std::endl;
    std::cout << "Experiment 5:" << std::endl;
    /**
     * Experiment 5: delete those movies with the attribute “numVotes” equal to 1,000, update the B+ tree accordingly, and report the following statistics:
		- the number of times that a node is deleted (or two nodes are merged) during the process of the updating the B+ tree
		- the number nodes of the updated B+ tree
		- the height of the updated B+ tree
		- the content of the root node and its 1st child node of the updated B+ tree
     */
    int pre = indexTree.getNumNodes();
    cout << "previous num nodes " << pre << endl;
    indexTree.remove(1000);
    int after = indexTree.getNumNodes();
    cout
            << "the number of times that a node is deleted (or two nodes are merged) during the process of the updating the B+ tree: "
            << pre - after << endl;
    cout << "after remove, num nodes " << after << endl;
    cout << "levels: " << indexTree.getLevels() << endl;
    cout << "root keys: ";
    for (size_t i = 0; i < indexTree.getRoot()->getKeys().size(); i++) {
        cout << indexTree.getRoot()->getKeys()[i] << " ";
    }
    cout << endl;
    Node *firstChild = indexTree.getRoot()->getChildNodes()[0];
    cout << "first child keys: ";
    for (size_t i = 0; i < firstChild->getKeys().size(); i++) {
        cout << firstChild->getKeys()[i] << " ";
    }
    cout << endl;

}

int main(int argc, char *argv[]) {
    assert(argc == 3 &&
           "invalid usage: ./main [storage_size_in_mb] [block_size_in_b]");

    // setup database
    std::cout << "setting storage size to " << argv[1] << " Mb" << std::endl;
    std::cout << "setting block size to " << argv[2] << " b" << std::endl;
    auto storage = Storage(/*total size*/ mbToBytes(std::atoi(argv[1])),
                                          std::atoi(argv[2]));
    auto bpIndexTree = BPTree(3);

    // read data from file
    std::cout << "loading data" << std::endl;
    auto entries = LoadEntryFromFileOrDie();


    //  run experiments
    //  TODO: this section was not tested as bp hasn't been finished yet
    run_experiments(storage, entries, bpIndexTree);


    std::cout << "the first entry has rating: " << entries[0].getRating()
              << std::endl;
    std::cout << "the first entry has numVotes: " << entries[0].numVotes
              << std::endl;

    return 0;
}
