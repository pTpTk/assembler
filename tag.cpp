#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <limits>
#include <cassert>
#include <vector>

#include "misc.h"

#define EMPTY_SYMTAB_EMTRY \
{ \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
}

#define CHECK(TAG, SIZE) \
    if(token == TAG) { \
        pos += SIZE; \
        nextLine(ifs); \
        continue; \
    }

std::unordered_map<std::string, int> tagMap;
std::string strTab;
std::vector<uint8_t> symTabLocal(0x18, 0);
std::vector<uint8_t> symTabGlobal;
std::vector<uint8_t> relText;

namespace {

struct RelEntry {
    std::string name;
    uint offset;

    RelEntry(std::string n, uint o) : name(n), offset(o) {}
};

std::unordered_set<std::string> globalTags;
std::unordered_set<std::string> calledTags;
std::unordered_map<std::string, int> tagIndexes;
std::vector<RelEntry> rels;

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

void readInput(std::ifstream& ifs) {
    std::string token;

    uint pos = 0;
    while(ifs >> token) {

        D("%2x: %s\n", pos, token.c_str());

        CHECK("addq"  , 4);
        CHECK("cdq"   , 1);
        CHECK("idivl" , 2);
        CHECK("imul"  , 3);
        CHECK("int"   , 2);
        CHECK("je"    , 2);
        CHECK("jmp"   , 2);
        CHECK("jne"   , 2);
        CHECK("movq"  , 3);
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

        if(token == "call") {
            std::string tag;
            ifs >> tag;

            calledTags.insert(tag);
            rels.emplace_back(tag, pos+1);

            pos += 5;
            continue;
        }

        if(token == ".globl") {
            std::string tag;
            ifs >> tag;

            globalTags.insert(tag);

            continue;
        }

        // tag definitions
        assert(token[token.size()-1] == ':');

        token.resize(token.size()-1);
        tagMap[token] = pos;

    }

    D("file length: 0x%x\n", pos);
}

void buildTabs() {
    strTab += '\0';
    int index = 1;

    for(const auto& [tag, val] : tagMap) {

        D("tag: %s, val: 0x%x\n", tag.c_str(), val);

        std::vector<uint8_t> symTabEntry = EMPTY_SYMTAB_EMTRY;
        uint32_t& st_name  = (uint32_t&)symTabEntry[0x00];
        uint64_t& st_value = (uint64_t&)symTabEntry[0x08];
        st_name  = strTab.size();
        st_value = val;

        if (isGlobal(tag)) {
            symTabEntry[4] = 0x10;
            merge(symTabGlobal, symTabEntry);
        }
        else {
            merge(symTabLocal, symTabEntry);
        }

        calledTags.erase(tag);

        tagIndexes[tag] = index;
        ++index;

        strTab += tag;
        strTab += '\0';
    }

    for(auto& tag : calledTags) {
        std::vector<uint8_t> symTabEntry = EMPTY_SYMTAB_EMTRY;
        uint pos = strTab.size();
        uint* uint_ptr = (uint*)symTabEntry.data();
        uint_ptr[0] = pos;
        symTabEntry[4] = 0x10;
        symTabEntry[6] = 0x00;
        merge(symTabGlobal, symTabEntry);

        tagIndexes[tag] = index;
        ++index;

        strTab += tag;
        strTab += '\0';
    }

    D("local sym table has %lu entries, "
      "global sym table has %lu entries\n",
      symTabLocal.size() >> 4, symTabGlobal.size() >> 4);
}

void buildRel() {
    D("relocation record:\n");
    
    for(auto& rel : rels) {
        std::vector<uint8_t> relEntry(8,0);
        uint32_t& r_offset = (uint&)relEntry[0];
        uint8_t&  r_type   =        relEntry[4];
        uint8_t&  r_sym    =        relEntry[5];
        r_offset = rel.offset;
        r_type   = 0x02;
        r_sym    = tagIndexes[rel.name];

        merge(relText, relEntry);

        D("%s: %x, tagID: %d\n", rel.name.c_str(), 
            rel.offset, tagIndexes[rel.name]);
    }
}

} // namespace

void collectTags(std::ifstream& ifs) {
    readInput(ifs);
    buildTabs();
    buildRel();
}
