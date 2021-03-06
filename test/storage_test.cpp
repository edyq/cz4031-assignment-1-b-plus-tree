//
// Created by Maokang Dou on 30/9/21.
//
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

#include "storage.h"
#include "utils.h"

using namespace std;

void testStorageQuery() {
    Storage storage(19 * 10 * 5, 19 * 3);

    std::vector<Entry> entries;
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '1'}, 2, 1});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '2'}, 3, 10});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '3'}, 4, 50});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '4'}, 5, 100});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '5'}, 6, 200});

    std::unordered_map<std::string, std::shared_ptr<Block>> blocks;
    for (auto entry: entries) {
        std::string s;
        for (auto c: entry.tconst) s += c;
        blocks[s] = storage.insertEntry(entry);
    }

    for (auto block: blocks) std::cout << block.second << std::endl;

    auto gotEntries = storage.query(blocks["tt0000001"], 1, 49);
    for (auto entry: gotEntries) std::cout << entry.toString() << std::endl;
}


void testStorageDelete() {
    // Create mock storage
    Storage storage(18 * 3 * 5, 18 * 3);

    // Insert mock data
    std::vector<Entry> entries;
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '1'}, 2, 1});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '2'}, 3, 10});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '3'}, 4, 50});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '4'}, 5, 100});
    entries.push_back(Entry{{'t', 't', '0', '0', '0', '0', '0', '0', '5'}, 6, 200});

    std::unordered_map<std::string, std::shared_ptr<Block>> blocks;
    for (auto entry: entries) {
        std::string s;
        for (auto c: entry.tconst) s += c;
        blocks[s] = storage.insertEntry(entry);
    }

    for (auto entry: entries) std::cout << blocks[entry.toString().substr(0, 9)] << std::endl;

    // Delete
    bool result = storage.deleteEntry(blocks[entries[0].toString().substr(0, 9)], entries[0].tconst);
    std::cout << result << std::endl;
    auto gotEntries = storage.query(blocks["tt0000001"], 1, 49);
    for (auto entry: gotEntries) std::cout << entry.toString() << std::endl;

}
