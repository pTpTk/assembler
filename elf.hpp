#pragma once

#include <cstdint>
#include <array>

constexpr std::array<uint8_t, 0x34> file_header = 
{
    0x7F, 0x45, 0x4C, 0x46, 0x01, 0x01, 0x01, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 
    0x05, 0x00, 0x01, 0x00
};

constexpr std::array<uint8_t, 0x28> index0_section_header = {0};

constexpr std::array<uint8_t, 0x28> shstrtab_section_header = 
{
    0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x22, 0x01, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

constexpr std::array<uint8_t, 0x28> strtab_section_header = 
{
    0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0C, 0x01, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

constexpr std::array<uint8_t, 0x28> symtab_section_header = 
{
    0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xAC, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00
};

constexpr std::array<uint8_t, 0x28> text_section_header = 
    {};

constexpr std::array<uint8_t, 0x21> shstrtab = 
{
    0x00, 0x2E, 0x73, 0x79, 0x6D, 0x74, 0x61, 0x62, 
    0x00, 0x2E, 0x73, 0x74, 0x72, 0x74, 0x61, 0x62, 
    0x00, 0x2E, 0x73, 0x68, 0x73, 0x74, 0x72, 0x74, 
    0x61, 0x62, 0x00, 0x2E, 0x74, 0x65, 0x78, 0x74, 
    0x00
};