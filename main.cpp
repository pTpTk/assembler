#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "misc.h"

std::unordered_map<std::string, int> tagMap;
std::vector<uint8_t> insts;

int main(int argc, char** argv) {
    assert(argc == 3);

    std::ifstream ifs1(argv[1]);
    std::ifstream ifs2(argv[1]);

    std::ofstream ofs(argv[2], std::ios::binary);

    collectTags(ifs1);
    assemble(ifs2);

    write(ofs, file_header);
    write(ofs, shstrtab);
    write(ofs, insts);
    write(ofs, index0_section_header);

}