#include <cassert>
#include <iostream>

int main(int argc, char* argv[]) {
    assert(argc == 2 && "invalid usage: ./main [size_in_mb]");
    std::cout << "setting storage size to " << argv[1] << std::endl;
    return 0;
}
