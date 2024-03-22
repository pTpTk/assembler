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

int main(int argc, char** argv) {
    assert(argc == 3);

    std::ifstream ifs1(argv[1]);
    std::ifstream ifs2(argv[1]);

    std::ofstream ofs(argv[2], std::ios::binary);

    collectTags(ifs1);
    assemble(ifs2);

    // write(ofs, file_header);
    // write(ofs, shstrtab);
    // writeInsts(ofs);
    // writeStrTab(ofs);
    // align(ofs, 4);
    // writeSymTab(ofs);
    // align(ofs, 8);
    // write(ofs, index0_section_header);
    // write(ofs, shstrtab_section_header);
    // write(ofs, text_section_header);
    // write(ofs, strtab_section_header);
    // write(ofs, symtab_section_header);

}