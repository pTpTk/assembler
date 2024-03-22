#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>


#ifdef DEBUG
    #define D(...) printf(__VA_ARGS__)
#else
    #define D(...)
#endif

// extern std::vector<uint8_t> file_header;
// extern std::vector<uint8_t> shstrtab;
// extern std::vector<uint8_t> index0_section_header;
// extern std::vector<uint8_t> shstrtab_section_header;
// extern std::vector<uint8_t> text_section_header;
// extern std::vector<uint8_t> strtab_section_header;
// extern std::vector<uint8_t> symtab_section_header;

// void collectTags(std::ifstream& ifs);
// void assemble(std::ifstream& ifs);
// void write(std::ofstream& ofs, const std::vector<uint8_t>& v);
// void writeInsts(std::ofstream& ofs);
// void writeStrTab(std::ofstream& ofs);
// void writeSymTab(std::ofstream& ofs);
// void align(std::ofstream& ofs, int i);
