#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>

#include "storage.h"


inline size_t mbToBytes(size_t size_in_mb) { return size_in_mb * 1048576; }

inline double bytesToMb(size_t size_in_bytes) { return (double) size_in_bytes / 1048576; }

template<typename T>
inline void printVector(std::vector<T> nums, std::string delimiter = " | ") {
    if (nums.size() == 0) return;
    const auto termination_idx = nums.size() - 1;
    for (size_t idx = 0; idx < termination_idx; idx++) {
        std::cout << std::to_string(nums.at(idx)) << delimiter;
    }
    std::cout << std::to_string(nums.at(termination_idx)) << std::endl;
}


template<typename T>
inline T charsToUnsignedNum(const char *buf) {
    T ret = 0;
    for (size_t i = 0; i < sizeof(T); i++) {
        ret |= (T) (unsigned char) buf[i]
                << (8 * i);
    }
    return ret;
}

template<typename T>
inline void unsignedNumToChars(char *buf, const T number) {
    // note that this is little endian
    for (size_t i = 0; i < sizeof(T); i++) {
        buf[i] = (unsigned char) (number >> (8 * i));
    }
}

/**
 * Convert data to Entry object.
 * @param ptr pointer to data
 * @return Entry object
 */
inline Entry charsToEntry(char *ptr) {
    Entry entry{};

    // Get tconst
    memcpy(entry.tconst, ptr, Entry::tconstSize);
    ptr += Entry::tconstSize;

    // Get rating
    entry.rating = charsToUnsignedNum<Entry::ratingType>(ptr);
    ptr += sizeof entry.rating;

    // Get numVotes
    entry.numVotes = charsToUnsignedNum<Entry::numVotesType>(ptr);
    ptr += sizeof entry.numVotes;
    return entry;
}

#endif
