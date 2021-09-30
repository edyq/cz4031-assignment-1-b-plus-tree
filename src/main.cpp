#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

#include "storage.h"
#include "utils.h"


void testStorageQuery() {
	Storage storage(19 * 10 * 5, 19 * 3);

	std::vector<Entry> entries;
	entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '1'}, 2, 1});
	entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '2'}, 3, 10});
	entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '3'}, 4, 50});
	entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '4'}, 5, 100});
	entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '5'}, 6, 200});

	std::unordered_map<std::string, std::shared_ptr<Block>> blocks;
	for (auto entry : entries) {
		std::string s;
		for (auto c : entry.tconst) s += c;
		blocks[s] = storage.insertEntry(entry);
	}

	for (auto block : blocks) std::cout << block.second << std::endl;

	auto gotEntries = storage.query(blocks["tt0000001"], 1, 49);
	for (auto entry : gotEntries) std::cout << entry.toString() << std::endl;
}

int main(int argc, char* argv[]) {
//    assert(argc == 3 &&
//           "invalid usage: ./main [storage_size_in_mb] [block_size_in_b]");
//    std::cout << "setting storage size to " << argv[1] << " Mb" << std::endl;
//    std::cout << "setting block size to " << argv[2] << " b" << std::endl;
//    auto storage = Storage(/*total size*/ mbToBytes(std::atoi(argv[1])),
//                           std::atoi(argv[2]));
//    return 0;

	testStorageQuery();
}
