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
    Block(char* ptr, size_t size) : ptr_(ptr), write_ptr_(ptr), size_(size){};
    ~Block(){};

    /**
     * @brief get block data
     *
     * @return void*
     */
    void* getPtr();

    bool writeData(const char* buf, size_t length) {
        if (getRemainingSpace() < length) {
            std::cout << "insufficient space inside the block! "
                      << getRemainingSpace() << " v.s. " << length << std::endl;
            return false;
        }
        std::memcpy(write_ptr_, buf, length);
        write_ptr_ += length;
        return true;
    };

    bool deleteData(char* start, size_t length);

    size_t getRemainingSpace() { return size_ - (write_ptr_ - ptr_); }

   private:
    void resetWritePtr();
    char* ptr_;
    char* write_ptr_;
    size_t size_;
};

#endif
