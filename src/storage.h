#ifndef STORAGE_H
#define STORAGE_H

#include <stdlib.h>

#include <string>
#include <memory>
#include <vector>

#include "block.h"

struct Entry {
	typedef uint8_t ratingType;
	typedef uint32_t numVotesType;
	static constexpr int tconstSize = 9;
	static constexpr int size = 18;

    // tconst	averageRating	numVotes
    char tconst[tconstSize];
    ratingType rating;  // with one implied decimal
    numVotesType numVotes;  // uint16_t may leads to overflow

    std::string toString() {
    	std::string s;
    	s += tconst;
    	s += " " + std::to_string(rating);
    	s += " " + std::to_string(numVotes);
    	return s;
    }

    float getRating() {
        float res = 0.;
        int num_rat = rating;
        res += num_rat / 10;
        res += num_rat % 10 * 0.1;
        return res;
    }
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

    /**
     * Get entries whose numVotes is within [lowerBound, upperBound].
     * @param block pointer to the block to search
     * @param lowerBound lower bound of numVotes
     * @param upperBound upper bound of numVotes
     * @return a vector of Entry objects
     */
    std::vector<Entry> query(std::shared_ptr<Block> block, double lowerBound, double upperBound);

   private:
    size_t total_size_;
    size_t block_size_;
    char* arena_;
    const size_t entry_size_ = Entry::size;
    std::vector<std::shared_ptr<Block>> blocks_;
};

#endif
