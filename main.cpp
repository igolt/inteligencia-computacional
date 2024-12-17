#include "Instance.hpp"

#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>

int main(int argc, const char *argv[])
{
    const char *progName = argv[0];

    if (argc != 2) {
        std::cerr << "usage: " << progName << " FILE\n";
        return 1;
    }
    std::cout << Instance::fromFile(argv[1]) << '\n';
    return 0;
}
