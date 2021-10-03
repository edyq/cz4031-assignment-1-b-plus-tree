#include "storage.h"

#include <cmath>
#include <algorithm>
#include <cstring>
#include <iostream>

#include "utils.h"

Storage::Storage(size_t total_size, size_t block_size): total_size_(total_size), block_size_(block_size) {
    auto block_num = 1 + ((total_size - 1) / block_size);
    blocks_.reserve(block_num);
    arena_ = new char[total_size]();
    auto ptr = arena_;
    for (size_t i = 0; i < block_num; i++) {
        blocks_.emplace_back(std::make_shared<Block>(ptr, block_size));
        ptr += block_size;
    }
}

Storage::~Storage() { delete arena_; }

std::shared_ptr<Block> Storage::insertEntry(Entry entry) {
    // step 1: serialize entry
    // Q: why isn't it done inside Entry?
    // A: Storage should be the one to decide how to serialize its data
    auto serlized_entry = new char[entry_size_];
    auto ptr = serlized_entry;
    std::memcpy(ptr, entry.tconst, 10);
    ptr += 10;
    unsignedNumToChars<uint8_t>(ptr, entry.rating);
    unsignedNumToChars<uint32_t>(++ptr, entry.numVotes);

    // step 2: find a proper block
    auto target_block = std::find_if(
        begin(blocks_), end(blocks_), [this](std::shared_ptr<Block> blk) {
            return blk->getRemainingSpace() >= entry_size_;
        });
    if (target_block == std::end(blocks_)) {
        std::cout << "no more available blocks! storage if full!" << std::endl;
        return nullptr;
    }

    // step 3. insert data into block
    (*target_block)->writeData(serlized_entry, entry_size_);
    delete[] serlized_entry;

    return *target_block;
}

std::vector<std::shared_ptr<Block>> Storage::initBatchInsertEntries(const std::vector<Entry>& entries) {
    std::vector<std::shared_ptr<Block>> target_blocks;
    size_t entry_per_block = std::floor((double) block_size_ / Entry::size);
    std::cout << "entry_per_block " << entry_per_block << std::endl;
    size_t block_idx = 0;
    auto serlized_entry = new char[Entry::size];
    for (size_t i = 0; i < entries.size(); i++) {
        if (i != 0 && i % entry_per_block == 0) {
            block_idx++;
        }
        auto ptr = serlized_entry;
        std::memcpy(ptr, entries[i].tconst, 10);
        ptr += 10;
        unsignedNumToChars<uint8_t>(ptr, entries[i].rating);
        unsignedNumToChars<uint32_t>(++ptr, entries[i].numVotes);
        blocks_[block_idx]->writeData(serlized_entry, Entry::size);
        target_blocks.push_back(blocks_[block_idx]);
    }
    delete[] serlized_entry;
    return target_blocks;
};

void advanceToNextEntry(char*& ptr) { ptr += Entry::size; }

bool Storage::deleteEntry(std::shared_ptr<Block> block, const char* tconst) {
    auto start = block->getPtr();
    for (auto ptr = start; ptr < block->getWritePtr();
         advanceToNextEntry(ptr)) {
        Entry entry = charsToEntry(ptr);
        if (!memcmp(entry.tconst, tconst, Entry::tconstSize)) {
            block->deleteData(ptr, Entry::size);
            return true;
        }
    }
    return false;
}

std::vector<Entry> Storage::query(std::shared_ptr<Block> block,
                                  uint32_t lowerBound, uint32_t upperBound) {
    std::vector<Entry> result;
    auto start = block->getPtr(), end = block->getWritePtr();
    for (auto ptr = start; ptr < end; advanceToNextEntry(ptr)) {
        Entry entry = charsToEntry(ptr);

        // check if entry numVotes is within [lowerBound, upperBound]
        if (entry.numVotes >= lowerBound && entry.numVotes <= upperBound) {
            result.push_back(entry);
        }
    }
    return result;
}

void Storage::print(std::shared_ptr<Block> block) {
    auto start = block->getPtr(), end = block->getWritePtr();
    for (auto ptr = start; ptr < end; advanceToNextEntry(ptr)) {
        Entry entry = charsToEntry(ptr);
        std::cout << entry.getTconst() << " | ";
    }
    std::cout << std::endl;
}
