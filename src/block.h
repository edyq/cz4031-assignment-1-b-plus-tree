#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>

#include <cstring>
#include <iostream>

class Block {
public:
    /**
     * @brief Construct a new Block object
     *
     * @param ptr pointer that marks the start of the block
     * @param size block size
     */
    Block(char *ptr, size_t size) : ptr_(ptr), write_ptr_(ptr), size_(size) {};

    ~Block() {};

    /**
     * @brief get block data
     *
     * @return char*
     */
    char *getPtr() { return ptr_; }

    char *getWritePtr() { return write_ptr_; }

    size_t getSize() { return size_; }

    bool writeData(const char *buf, size_t length) {
        if (getRemainingSpace() < length) {
            std::cout << "insufficient space inside the block! "
                      << getRemainingSpace() << " v.s. " << length << std::endl;
            return false;
        }
        std::memcpy(write_ptr_, buf, length);
        write_ptr_ += length;
        return true;
    };

    bool deleteData(char *start, size_t length) {
        auto cutoff_ptr = start + length;
        if (cutoff_ptr == write_ptr_) {
            // optimal case, just trunc data
            write_ptr_ -= length;
            return true;
        } else if (cutoff_ptr < write_ptr_) {
            // copy data to front
            std::memcpy(start, cutoff_ptr, write_ptr_ - cutoff_ptr);
            write_ptr_ -= length;
            return true;
        } else {
            std::cout << "illegal deletion: requested deleting range overflowed"
                      << std::endl;
            // nothing done
            return false;
        }
    };

    size_t getRemainingSpace() { return size_ - (write_ptr_ - ptr_); }

private:
    void resetWritePtr();

    char *ptr_;
    char *write_ptr_;
    size_t size_;
};

#endif
