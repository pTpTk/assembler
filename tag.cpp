#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <limits>
#include <cassert>

#define CHECK(TAG, SIZE) \
    if(token == TAG) { \
        pos += SIZE; \
        nextLine(ifs); \
        continue; \
    }


#ifdef DEBUG
    #define D(...) printf(__VA_ARGS__)

    #define PRINT() { \
    printf("0x"); \
    for(auto i : inst) \
        printf("%02x ", i); \
    printf("\n"); \
    }

#else
    #define D(...)
    #define PRINT()
#endif

std::unordered_map<std::string, uint> tagMap;

inline void nextLine(std::ifstream& ifs) {
    ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void collectTags(std::ifstream& ifs) {
    std::string token;

    uint pos = 0;
    while(ifs >> token) {

        CHECK(".globl", 0);
        // TODO::call
        CHECK("cdq"   , 1);
        CHECK("cmpl"  , 2);
        CHECK("idivl" , 2);
        CHECK("imul"  , 3);
        // TODO::je,jump,jne
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

        assert(token[token.size()-1] == ':');

        token.resize(token.size()-1);
        tagMap[token] = pos;

    }

    for(const auto& [key, val] : tagMap) {
        D("tag: %s, val: 0x%x\n", key.c_str(), val);
    }

    D("file length: 0x%x\n", pos);

}