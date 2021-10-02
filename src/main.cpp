#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <set>

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
                std::strcpy(current_entry.tconst, infile.c_str());
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

void run_exp_3(BPTree& indexTree) {
	auto result = indexTree.search(500, 500);
	result.
}

void run_experiments(Storage& storage, std::vector<Entry>& entries, BPTree& indexTree) {
    std::set<std::shared_ptr<Block>> used_blocks;
    for (auto& entry: entries) {
        auto block_ptr = storage.insertEntry(entry);
        indexTree.insert(entry.numVotes, block_ptr);
        used_blocks.insert(block_ptr);
    }
    std::cout << "=========================" << std::endl;
    std::cout << " Experiment 1:" << std::endl;
    std::cout << "the number of blocks: " << used_blocks.size();
    std::cout << "the size of database (in terms of MB)" << bytesToMb(entries.size() * Entry::size);

    std::cout << "=========================" << std::endl;
    std::cout << " Experiment 2:" << std::endl;
    std::cout << "parameter n of the B+ tree" << indexTree.getMaxKeys();
    std::cout << "the number of nodes of the B+ tree" << indexTree.getNumNodes();
    std::cout << "the height of the B+ tree, i.e., the number of levels of the B+ tree" << indexTree.getLevels();
    auto rootNode = indexTree.getRoot();
    std::cout << "the content of the root node: ";
    printVector<uint32_t>(rootNode->getKeys());

    // Experiment 3
    std::cout << "=========================" << std::endl;
    std::cout << " Experiment 3:" << std::endl;
    /**
     * Experiment 3: retrieve those movies with the “numVotes” equal to 500 and report the following statistics:
		- the number and the content of index nodes the process accesses
		- the number and the content of data blocks the process accesses
		- the average of “averageRating’s” of the records that are returned
     */


    // Experiment 4
    std::cout << "=========================" << std::endl;
    std::cout << " Experiment 4:" << std::endl;
    /**
     *  Experiment 4: retrieve those movies with the attribute “numVotes” from 30,000 to 40,000, both inclusively and report the following statistics:
		- the number and the content of index nodes the process accesses
		- the number and the content of data blocks the process accesses
		- the average of “averageRating’s” of the records that are returned
     */


    // Experiment 5
    std::cout << "=========================" << std::endl;
    std::cout << " Experiment 5:" << std::endl;
    /**
     * Experiment 5: delete those movies with the attribute “numVotes” equal to 1,000, update the B+ tree accordingly, and report the following statistics:
		- the number of times that a node is deleted (or two nodes are merged) during the process of the updating the B+ tree
		- the number nodes of the updated B+ tree
		- the height of the updated B+ tree
		- the content of the root node and its 1st child node of the updated B+ tree
     */
}

int main(int argc, char* argv[]) {
    assert(argc == 3 &&
           "invalid usage: ./main [storage_size_in_mb] [block_size_in_b]");

    // setup database
    std::cout << "setting storage size to " << argv[1] << " Mb" << std::endl;
    std::cout << "setting block size to " << argv[2] << " b" << std::endl;
    auto storage = Storage(/*total size*/ mbToBytes(std::atoi(argv[1])),
                           std::atoi(argv[2]));
    auto bpIndexTree = BPTree(10);

    // read data from file
    std::cout << "loading data" << std::endl;
    auto entries = LoadEntryFromFileOrDie();


    // run experiments
    // TODO: this section was not tested as bp hasn't been finished yet
    run_experiments(storage, entries, bpIndexTree);


    std::cout << "the first entry has rating: " << entries[0].getRating()
              << std::endl;
    std::cout << "the first entry has numVotes: " << entries[0].numVotes
              << std::endl;


//    BPTree t = BPTree(5);
//    t.insert(11);
//    t.insert(23);
//    t.insert(34);
//    t.insert(36);
//    vector<uint32_t> keys = t.getRoot()->getKeys();
//    for (int i=0; i<keys.size(); i++){
//        cout << keys[i] << " ";
//    }
//    cout << endl;
//    t.remove(11);
//    keys = t.getRoot()->getKeys();
//    for (int i=0; i<keys.size(); i++){
//        cout << keys[i] << " ";
//    }
//    cout << endl;
    return 0;
}
