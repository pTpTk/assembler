#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "misc.h"

void collectTags(std::ifstream& ifs);
void assemble(std::ifstream& ifs);
void writeElf(std::ofstream& ofs);

int main(int argc, char** argv) {
    assert(argc == 3);

    std::ifstream ifs1(argv[1]);
    std::ifstream ifs2(argv[1]);

    std::ofstream ofs(argv[2], std::ios::binary);

    collectTags(ifs1);
    assemble(ifs2);
    writeElf(ofs);

}