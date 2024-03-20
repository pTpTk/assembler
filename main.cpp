#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "misc.h"
#include "elf.h"

std::unordered_map<std::string, int> tagMap;
std::vector<uint8_t> insts;

int main(int argc, char** argv) {
    assert(argc == 2);

    std::ifstream ifs1(argv[1]);
    std::ifstream ifs2(argv[1]);

    collectTags(ifs1);
    assemble(ifs2);

    // for(auto c : shstrtab) {
    //     printf("%c", c);
    // }
    // printf("\n");

}