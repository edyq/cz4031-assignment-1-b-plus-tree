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
	static constexpr char tconstTerminator = '|';
	static constexpr int tconstSize = 10;
	static constexpr int size = 19;

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

    std::string getTconst() {
        auto tmpChar = new char[tconstSize + 1]();
        for (size_t i = 0; i < tconstSize; i++) {
            if (tconst[i] == tconstTerminator) {
                tmpChar[i] = '\0';
                break;
            } else {
                tmpChar[i] = tconst[i];
                if (i == (tconstSize - 1)) {
                    tmpChar[i + 1] = '\0';
                }
            }
        }
        std::string convertedTconst(tmpChar);
        delete tmpChar;
        return convertedTconst;
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
    std::vector<Entry> query(std::shared_ptr<Block> block, uint32_t lowerBound, uint32_t upperBound);

    static void print(std::shared_ptr<Block> block);

   private:
    size_t total_size_;
    size_t block_size_;
    char* arena_;
    const size_t entry_size_ = Entry::size;
    std::vector<std::shared_ptr<Block>> blocks_;
};

#endif
