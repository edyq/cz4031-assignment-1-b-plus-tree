#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>

class Block {
   public:
   /**
    * @brief Construct a new Block object
    * 
    * @param ptr pointer that marks the start of the block
    * @param size block size
    */
    Block(void* ptr, size_t size);
    ~Block();

    /**
     * @brief get block data
     * 
     * @return void* 
     */
    void* getPtr();

    bool deleteData(void* start, size_t length);

   private:
    void resetWritePtr();
    void* ptr_;
    void* write_ptr_;
    size_t size_;
};

#endif
