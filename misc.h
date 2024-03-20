#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

#define CHECK(TAG, SIZE) \
    if(token == TAG) { \
        pos += SIZE; \
        nextLine(ifs); \
        continue; \
    }


#ifdef DEBUG
    #define D(...) printf(__VA_ARGS__)

    #define PRINT() { \
    printf("%2lx: ", insts.size()); \
    printf("0x"); \
    for(auto i : inst) \
        printf("%02x ", i); \
    printf("\n"); \
    }

#else
    #define D(...)
    #define PRINT()
#endif

extern std::unordered_map<std::string, int> tagMap;
extern std::vector<uint8_t> insts;

void collectTags(std::ifstream& ifs);
void assemble(std::ifstream& ifs);
