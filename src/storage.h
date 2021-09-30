#ifndef STORAGE_H
#define STORAGE_H

#include <stdlib.h>

#include <memory>
#include <vector>

#include "block.h"

struct Entry {
	typedef uint8_t ratingType;
	typedef uint32_t numVotesType;
	static constexpr int tconstSize = 9;
	static constexpr int size = 19;

    // tconst	averageRating	numVotes
    char tconst[tconstSize];
    ratingType rating;  // with one implied decimal
    numVotesType numVotes;  // uint16_t may leads to overflow

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
     * block. returns nullptr if no more available blocks
     * \warning assuming one entry cannot be seperated into blocks
     *
     * @param entry
     * @return std::shared_ptr<Block>
     */
    std::shared_ptr<Block> insertEntry(Entry entry);

    bool deleteEntry(std::shared_ptr<Block>, const char* tconst);

    std::vector<Entry> query(std::shared_ptr<Block> block, double lowerBound, double upperBound);

   private:
    size_t total_size_;
    size_t block_size_;
    char* arena_;
    const size_t entry_size_ = 19;
    std::vector<std::shared_ptr<Block>> blocks_;
};

#endif
