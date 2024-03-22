#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <limits>
#include <cassert>

#include "misc.h"

#define EMPTY_SYMTAB_EMTRY \
{ \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00 \
}

#define CHECK(TAG, SIZE) \
    if(token == TAG) { \
        pos += SIZE; \
        nextLine(ifs); \
        continue; \
    }

std::unordered_map<std::string, int> tagMap;
std::string strTab;
std::vector<uint8_t> symTab(0x10, 0);

namespace {

std::unordered_set<std::string> globalTags;

inline void nextLine(std::ifstream& ifs) {
    ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

inline void merge(std::vector<uint8_t>& v1, 
                  std::vector<uint8_t>& v2) {
    v1.insert(v1.end(), v2.begin(), v2.end());
}

inline bool isGlobal(std::string str) {
    return globalTags.find(str) != globalTags.end();
}

void buildTabs() {
    for(const auto& [tag, val] : tagMap) {

        D("tag: %s, val: 0x%x\n", tag.c_str(), val);

        strTab += '\0';

        std::vector<uint8_t> symTabEntry = EMPTY_SYMTAB_EMTRY;
        uint pos = strTab.size();
        uint* uint_ptr = (uint*)symTabEntry.data();
        uint_ptr[0] = pos;
        uint_ptr[1] = val;
        if (isGlobal(tag)) symTabEntry[0x0C] = 0x10;

        merge(symTab, symTabEntry);

        strTab += tag;
    }
}

} // namespace

void collectTags(std::ifstream& ifs) {
    std::string token;

    uint pos = 0;
    while(ifs >> token) {

        D("%2x: %s\n", pos, token.c_str());

        // TODO::call
        CHECK("cdq"   , 1);
        CHECK("idivl" , 2);
        CHECK("imul"  , 3);
        CHECK("je"    , 2);
        CHECK("jmp"   , 2);
        CHECK("jne"   , 2);
        CHECK("neg"   , 2);
        CHECK("not"   , 2);
        CHECK("pop"   , 1);
        CHECK("push"  , 1);
        CHECK("ret"   , 1);
        CHECK("sete"  , 3);
        CHECK("setg"  , 3);
        CHECK("setg"  , 3);
        CHECK("setge" , 3);
        CHECK("setl"  , 3);
        CHECK("setle" , 3);
        CHECK("setne" , 3);
        CHECK("subl"  , 2);

        if(token == "addl") {
            std::string arg1;
            ifs >> arg1;

            if(arg1[0] == '$') {
                pos += 3;
                nextLine(ifs);
                continue;
            }
            else {
                pos += 2;
                nextLine(ifs);
                continue;
            }
        }

        if(token == "cmpl") {
            std::string arg1;
            ifs >> arg1;

            if(arg1[0] == '$') {
                pos += 3;
                nextLine(ifs);
                continue;
            }
            else {
                pos += 2;
                nextLine(ifs);
                continue;
            }
        }

        if(token == "movl") {
            std::string arg1, arg2;
            ifs >> arg1 >> arg2;

            if(arg1[0] == '%' && arg2[0] == '%') {
                pos += 2;
                continue;
            }
            else if(arg1[0] == '$') {
                pos += 5;
                continue;
            }
            else {
                pos += 3;
                continue;
            }
        }

        if(token == ".globl") {
            std::string tag;
            ifs >> tag;

            globalTags.insert(tag);

            continue;
        }

        assert(token[token.size()-1] == ':');

        token.resize(token.size()-1);
        tagMap[token] = pos;

    }

    buildTabs();

    D("file length: 0x%x\n", pos);

}
