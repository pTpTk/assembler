#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>

#include "misc.h"

extern std::vector<uint8_t> insts;
extern std::string strTab;
extern std::vector<uint8_t> symTabLocal;
extern std::vector<uint8_t> symTabGlobal;
extern std::vector<uint8_t> relText;

namespace {

std::vector<uint8_t> file_header = 
{
    0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x40, 0x00, 0x06, 0x00, 0x01, 0x00
};

std::string shstrtab("\0.symtab\0.strtab\0.shstrtab\0.rel.text\0", 38);

class Section_Header
{
  public:
    std::vector<uint8_t> header = std::vector<uint8_t>(0x40, 0);
    uint32_t& sh_name      = (uint32_t&)header[0x00];
    uint32_t& sh_type      = (uint32_t&)header[0x04];
    uint64_t& sh_flags     = (uint64_t&)header[0x08];
    uint64_t& sh_offset    = (uint64_t&)header[0x18];
    uint64_t& sh_size      = (uint64_t&)header[0x20];
    uint32_t& sh_link      = (uint32_t&)header[0x28];
    uint32_t& sh_info      = (uint32_t&)header[0x2c];
    uint64_t& sh_addralign = (uint64_t&)header[0x30];
    uint64_t& sh_entsize   = (uint64_t&)header[0x38];
    
    char* data() { return (char*)header.data(); }
};

Section_Header index0_section_header;
Section_Header shstrtab_section_header;
Section_Header text_section_header;
Section_Header strtab_section_header;
Section_Header symtab_section_header;
Section_Header rel_text_section_header;

// std::vector<uint8_t> index0_section_header(0x28, 0);

// std::vector<uint8_t> shstrtab_section_header = 
// {
//     0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//     0x34, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
//     0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };

// std::vector<uint8_t> text_section_header = 
// {
//     0x1F, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
//     0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x5C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };

// std::vector<uint8_t> strtab_section_header = 
// {
//     0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };

// std::vector<uint8_t> symtab_section_header = 
// {
//     0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
// };

// std::vector<uint8_t> rel_text_section_header = 
// {
//     0x1B, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
//     0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
//     0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00
// };

inline void write(std::ofstream& ofs, const std::vector<uint8_t>& v) {
    ofs.write((const char *)v.data(), v.size());
}

inline void write(std::ofstream& ofs, Section_Header& v) {
    ofs.write(v.data(), v.header.size());
}

inline void align4(uint& offset) {
    if(offset & 0b11) {
        offset >>= 2;
        ++offset;
        offset <<= 2;
    }
}

inline void align8(uint& offset) {
    if(offset & 0b111) {
        offset >>= 3;
        ++offset;
        offset <<= 3;
    }
}

void align(std::ofstream& ofs, int i) {
    uint pos = ofs.tellp();
    while(pos % i != 0) {
        pos++;
        ofs << '\0';
    }
}

void writeFileHeader(std::ofstream& ofs) {
    ofs.seekp(0);
    write(ofs, file_header);
}

void writeShStrTab(std::ofstream& ofs) {
    ofs.seekp(0x40);

    ofs << shstrtab;

    uint64_t end = ofs.tellp();

    shstrtab_section_header.sh_offset = 0x40;
    shstrtab_section_header.sh_size = end - 0x40;

    align(ofs, 4);
}

void writeText(std::ofstream& ofs) {
    uint start = ofs.tellp();

    write(ofs, insts);

    uint end = ofs.tellp();

    text_section_header.sh_offset = start;
    text_section_header.sh_size = end - start;

    align(ofs, 4);
}

void writeStrTab(std::ofstream& ofs) {
    uint start = ofs.tellp();

    ofs << strTab;

    uint end = ofs.tellp();

    strtab_section_header.sh_offset = start;
    strtab_section_header.sh_size = end - start;

    align(ofs, 4);
}

void writeSymTab(std::ofstream& ofs) {

    uint start = ofs.tellp();

    write(ofs, symTabLocal);
    write(ofs, symTabGlobal);

    uint end = ofs.tellp();

    symtab_section_header.sh_offset = start;
    symtab_section_header.sh_size = end - start;

    // uint* uint_ptr;
    // uint_ptr = (uint*)symtab_section_header.data();
    // uint_ptr[4] = start;
    // uint_ptr[5] = end - start;
    // uint_ptr[7] = symTabLocal.size() >> 4;

    align(ofs, 4);
}

void writeRelText(std::ofstream& ofs) {
    uint start = ofs.tellp();

    write(ofs, relText);

    uint end = ofs.tellp();

    rel_text_section_header.sh_offset = start;
    rel_text_section_header.sh_size = end - start;

    align(ofs, 8);
}

void writeShHeaders(std::ofstream& ofs) {
    uint64_t& e_shoff = (uint64_t&)file_header[0x28];
    e_shoff = ofs.tellp();

    // write(ofs, index0_section_header);
    write(ofs, shstrtab_section_header);
    // write(ofs, text_section_header);
    write(ofs, strtab_section_header);
    // write(ofs, symtab_section_header);
    // write(ofs, rel_text_section_header);
}

} // namespace

void writeElf(std::ofstream& ofs) {
    // setShOff();
    writeShStrTab(ofs);
    writeText(ofs);
    writeStrTab(ofs);
    writeSymTab(ofs);
    writeRelText(ofs);
    writeShHeaders(ofs);
    writeFileHeader(ofs);
}