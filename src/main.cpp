#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

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

int main(int argc, char* argv[]) {
    assert(argc == 3 &&
           "invalid usage: ./main [storage_size_in_mb] [block_size_in_b]");
    std::cout << "setting storage size to " << argv[1] << " Mb" << std::endl;
    std::cout << "setting block size to " << argv[2] << " b" << std::endl;
    auto storage = Storage(/*total size*/ mbToBytes(std::atoi(argv[1])),
                           std::atoi(argv[2]));
    std::cout << "ready to load" << std::endl;
    auto entries = LoadEntryFromFileOrDie();
    std::cout << "the first entry has rating: " << entries[0].getRating()
              << std::endl;
    std::cout << "the first entry has numVotes: " << entries[0].numVotes
              << std::endl;
    return 0;
}
