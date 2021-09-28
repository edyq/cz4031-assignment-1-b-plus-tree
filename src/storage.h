#ifndef STORAGE_H
#define STORAGE_H

#include <stdlib.h>

#include <memory>
#include <vector>

#include "block.h"

struct Entry {
    // tconst	averageRating	numVotes
    char tconst[9];
    uint8_t averageRatingInteger;
    uint8_t averageRatingDecimal;
    uint32_t numVotes;  // uint16_t may leads to overflow
};

class Storage {
   public:
    /**
     * @brief Construct a new Storage; init blocks
     *
     * @param total_size size in bytes
     * @param block_size size in bytes
     */
    Storage(size_t total_size, size_t block_size);
    ~Storage();

    /**
     * @brief insert a new entry to storage and returns pinter of the inserted
     * block
     * \warning assuming one entry cannot be seperated into blocks
     *
     * @param entry
     * @return std::shared_ptr<Block>
     */
    std::shared_ptr<Block> insertEntry(Entry entry);

    bool deleteEntry(std::shared_ptr<Block>, const char* tconst);

   private:
    size_t total_size_;
    size_t block_size_;
    void* arena_;
    std::vector<std::shared_ptr<Block>> blocks_;
};

#endif
