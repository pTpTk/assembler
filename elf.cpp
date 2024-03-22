#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>

#include "misc.h"

extern std::vector<uint8_t> insts;
extern std::string strTab;
extern std::vector<uint8_t> symTab;

namespace {

std::vector<uint8_t> file_header = 
{
    0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 
    0x05, 0x00, 0x01, 0x00
};

std::vector<uint8_t> shstrtab = 
{
    0x00, 0x2E, 0x73, 0x79, 0x6D, 0x74, 0x61, 0x62, 
    0x00, 0x2E, 0x73, 0x74, 0x72, 0x74, 0x61, 0x62, 
    0x00, 0x2E, 0x73, 0x68, 0x73, 0x74, 0x72, 0x74, 
    0x61, 0x62, 0x00, 0x2E, 0x74, 0x65, 0x78, 0x74, 
    0x00
};

std::vector<uint8_t> index0_section_header(0x28, 0);

std::vector<uint8_t> shstrtab_section_header = 
{
    0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x34, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

std::vector<uint8_t> text_section_header = 
{
    0x1B, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

std::vector<uint8_t> strtab_section_header = 
{
    0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

std::vector<uint8_t> symtab_section_header = 
{
    0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};

inline void write(std::ofstream& ofs, const std::vector<uint8_t>& v) {
    ofs.write((const char *)v.data(), v.size());
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

void setShOff() {
    uint offset = 0;

    offset += file_header.size();
    offset += shstrtab.size();
    align4(offset);

    offset += insts.size();
    align4(offset);
    
    offset += strTab.size();
    align4(offset);

    offset += symTab.size();
    align8(offset);

    D("file_header: 0x%lx, shstrtab: 0x%lx, insts: 0x%lx, "
      "strtab: 0x%lx, symtab: 0x%lx, shoff: 0x%x\n", 
      file_header.size(), shstrtab.size(), insts.size(),
      strTab.size(), symTab.size(), offset);

    uint* uint_ptr = (uint*)file_header.data();
    uint_ptr[8] = offset;

}

void align(std::ofstream& ofs, int i) {
    uint pos = ofs.tellp();
    while(pos % i != 0) {
        pos++;
        ofs << '\0';
    }
}

void writeFileHeader(std::ofstream& ofs) {
    write(ofs, file_header);
}

void writeShStrTab(std::ofstream& ofs) {
    write(ofs, shstrtab);
    align(ofs, 4);
}

void writeText(std::ofstream& ofs) {
    uint start = ofs.tellp();

    write(ofs, insts);

    uint end = ofs.tellp();

    uint* uint_ptr;
    uint_ptr = (uint*)text_section_header.data();
    uint_ptr[4] = start;
    uint_ptr[5] = end - start;

    align(ofs, 4);
}

void writeStrTab(std::ofstream& ofs) {
    uint start = ofs.tellp();

    ofs << strTab;

    uint end = ofs.tellp();

    uint* uint_ptr;
    uint_ptr = (uint*)strtab_section_header.data();
    uint_ptr[4] = start;
    uint_ptr[5] = end - start;

    align(ofs, 4);
}

void writeSymTab(std::ofstream& ofs) {

    uint start = ofs.tellp();

    write(ofs, symTab);

    uint end = ofs.tellp();

    uint* uint_ptr;
    uint_ptr = (uint*)symtab_section_header.data();
    uint_ptr[4] = start;
    uint_ptr[5] = end - start;
    uint_ptr[7] = symTab.size() >> 4;

    align(ofs, 8);
}

void writeShHeaders(std::ofstream& ofs) {
    write(ofs, index0_section_header);
    write(ofs, shstrtab_section_header);
    write(ofs, text_section_header);
    write(ofs, strtab_section_header);
    write(ofs, symtab_section_header);
}

} // namespace

void writeElf(std::ofstream& ofs) {
    setShOff();
    writeFileHeader(ofs);
    writeShStrTab(ofs);
    writeText(ofs);
    writeStrTab(ofs);
    writeSymTab(ofs);
    writeShHeaders(ofs);
}