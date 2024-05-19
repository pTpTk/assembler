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
    0x7F, 0x45, 0x4C, 0x46, 0x02, 0x01, 0x01, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x3E, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x40, 0x00, 0x06, 0x00, 0x01, 0x00
};

std::string shstrtab("\0.symtab\0.strtab\0.shstrtab\0.rela.text\0", 39);

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

inline void write(std::ofstream& ofs, const std::vector<uint8_t>& v) {
    ofs.write((const char *)v.data(), v.size());
}

inline void write(std::ofstream& ofs, Section_Header& v) {
    ofs.write(v.data(), v.header.size());
}

void align(std::ofstream& ofs, int i) {
    uint pos = ofs.tellp();
    while(pos % i != 0) {
        pos++;
    }
    ofs.seekp(pos);
}

void writeFileHeader(std::ofstream& ofs) {
    ofs.seekp(0);
    write(ofs, file_header);
}

void writeShStrTab(std::ofstream& ofs) {
    ofs.seekp(0x40);

    ofs << shstrtab;

    uint64_t end = ofs.tellp();

    shstrtab_section_header.sh_name      = 0x11;
    shstrtab_section_header.sh_type      = 0x03;
    shstrtab_section_header.sh_offset    = 0x40;
    shstrtab_section_header.sh_size      = end - 0x40;
    shstrtab_section_header.sh_addralign = 0x01;

    align(ofs, 4);
}

void writeText(std::ofstream& ofs) {
    uint start = ofs.tellp();

    write(ofs, insts);

    uint end = ofs.tellp();

    text_section_header.sh_name      = 0x20;
    text_section_header.sh_type      = 0x01;
    text_section_header.sh_flags     = 0x06;
    text_section_header.sh_offset    = start;
    text_section_header.sh_size      = end - start;
    text_section_header.sh_addralign = 0x01;

    align(ofs, 4);
}

void writeStrTab(std::ofstream& ofs) {
    uint start = ofs.tellp();

    ofs << strTab;

    uint end = ofs.tellp();

    strtab_section_header.sh_name      = 0x11;
    strtab_section_header.sh_type      = 0x03;
    strtab_section_header.sh_offset    = start;
    strtab_section_header.sh_size      = end - start;
    strtab_section_header.sh_addralign = 0x01;

    align(ofs, 4);
}

void writeSymTab(std::ofstream& ofs) {

    uint start = ofs.tellp();

    write(ofs, symTabLocal);
    write(ofs, symTabGlobal);

    uint end = ofs.tellp();

    symtab_section_header.sh_name      = 0x01;
    symtab_section_header.sh_type      = 0x02;
    symtab_section_header.sh_offset    = start;
    symtab_section_header.sh_size      = end - start;
    symtab_section_header.sh_link      = 0x03;
    symtab_section_header.sh_info      = symTabLocal.size() / 0x18;
    symtab_section_header.sh_addralign = 0x08;
    symtab_section_header.sh_entsize   = 0x18;

    align(ofs, 4);
}

void writeRelText(std::ofstream& ofs) {
    uint start = ofs.tellp();

    write(ofs, relText);

    uint end = ofs.tellp();

    rel_text_section_header.sh_offset = start;
    rel_text_section_header.sh_size = end - start;

    rel_text_section_header.sh_name      = 0x1B;
    rel_text_section_header.sh_type      = 0x04;
    rel_text_section_header.sh_flags     = 0x40;
    rel_text_section_header.sh_offset    = start;
    rel_text_section_header.sh_size      = end - start;
    rel_text_section_header.sh_link      = 0x04;
    rel_text_section_header.sh_info      = 0x02;
    rel_text_section_header.sh_addralign = 0x08;
    rel_text_section_header.sh_entsize   = 0x18;

    align(ofs, 16);
}

void writeShHeaders(std::ofstream& ofs) {
    uint64_t& e_shoff = (uint64_t&)file_header[0x28];
    e_shoff = ofs.tellp();

    write(ofs, index0_section_header);
    write(ofs, shstrtab_section_header);
    write(ofs, text_section_header);
    write(ofs, strtab_section_header);
    write(ofs, symtab_section_header);
    write(ofs, rel_text_section_header);
}

} // namespace

void writeElf(std::ofstream& ofs) {
    writeShStrTab(ofs);
    writeText(ofs);
    writeStrTab(ofs);
    writeSymTab(ofs);
    writeRelText(ofs);
    writeShHeaders(ofs);
    writeFileHeader(ofs);
}