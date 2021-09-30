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
    std::cout << "hey";
    std::ifstream fin(filename);
    assert(fin.good() && "failed to open file for data ");
    std::string infile;
    std::vector<Entry> contents;
    size_t counter = 0;
    Entry current_entry;
    std::cout << "about to start";
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

void run_experiment_1_insert_data(Storage& storage, std::vector<Entry>& entries, BPTree& indexTree) {
    std::cout << "inserting entries..." << std::endl;
    std::set<std::shared_ptr<Block>> used_blocks;
    for (auto& entry: entries) {
        auto block_ptr = storage.insertEntry(entry);
        indexTree.insert(entry.numVotes, block_ptr);
        used_blocks.insert(block_ptr);
    }
    std::cout << "total number of block used: " << used_blocks.size();
    std::cout << "size of database: " << bytesToMb(entries.size() * Entry::size);
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
    std::cout << "====================" << std::endl;
    std::cout << "running experment 1" << std::endl;
    run_experiment_1_insert_data(storage, entries, bpIndexTree);
    std::cout << "====================" << std::endl;

    return 0;
}
