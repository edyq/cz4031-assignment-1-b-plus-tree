#include <cassert>
#include <cstdlib>
#include <iostream>

#include "storage.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    assert(argc == 3 &&
           "invalid usage: ./main [storage_size_in_mb] [block_size_in_b]");
    std::cout << "setting storage size to " << argv[1] << " Mb" << std::endl;
    std::cout << "setting block size to " << argv[2] << " b" << std::endl;
    auto storage = Storage(/*total size*/ mbToBytes(std::atoi(argv[1])),
                           std::atoi(argv[2]));
    return 0;
}
