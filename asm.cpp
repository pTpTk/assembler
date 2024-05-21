#include <fstream>
#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "misc.h"

#ifdef DEBUG
    #define PRINT() { \
    printf("%2lx: ", insts.size()); \
    printf("0x"); \
    for(auto i : inst) \
        printf("%02x ", i); \
    printf("\n"); \
    }
#else
    #define PRINT()
#endif

std::vector<uint8_t> insts;
extern std::unordered_map<std::string, int> tagMap;
extern std::vector<uint8_t> text_section_header;

namespace {

inline uint8_t Reg2Int(std::string reg) {
    if(reg == "%eax") return 0;
    if(reg == "%ecx") return 1;
    if(reg == "%edx") return 2;
    if(reg == "%ebx") return 3;
    if(reg == "%esi") return 6;
    if(reg == "%edi") return 7;

    if(reg == "%rax") return 0;
    if(reg == "%rcx") return 1;
    if(reg == "%rdx") return 2;
    if(reg == "%rbx") return 3;
    if(reg == "%rsp") return 4;
    if(reg == "%rbp") return 5;
    if(reg == "%rsi") return 6;
    if(reg == "%rdi") return 7;

    if(reg == "%al") return 0;
    if(reg == "%cl") return 1;
    if(reg == "%dl") return 2;
    if(reg == "%bl") return 3;

    assert(false);
}

inline int Imm2Int(std::string num) {
    assert(num[0] == '$');
    num[0] = '0';
    return stoi(num);
}

inline void Get2Args(std::ifstream& ifs,
    std::string& arg1, std::string& arg2) {
    ifs >> arg1 >> arg2;
    
    // get rid of ','
    arg1.resize(arg1.size()-1);
}

inline void merge(std::vector<uint8_t>& v1, 
                  std::vector<uint8_t>& v2) {
    v1.insert(v1.end(), v2.begin(), v2.end());
}

// add imm, reg
// 0x83 0b11'000'<reg> imm
// add src, dst
// 0x01 0b11'<src>'<dst>
void addl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst;

    if(arg1[0] == '$') {
    // add imm, reg

        inst = {0x83, 0xc0, 0x00};

        uint8_t reg = Reg2Int(arg2);

        int imm = Imm2Int(arg1);
        // temp hack for now
        assert(imm == (int8_t)imm);

        inst[1] |= reg;
        inst[2] = imm;

        PRINT();

    }
    else {
    // add src, dst
        inst = {0x01, 0xc0};

        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst[1] |= (src << 3);
        inst[1] |= dst;

        PRINT();

    }

    merge(insts, inst);
}

void addq(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst{0x48, 0x83, 0xc0, 0x00};

    uint8_t reg = Reg2Int(arg2);

    int imm = Imm2Int(arg1);
    // temp hack for now
    assert(imm == (int8_t)imm);

    inst[2] |= reg;
    inst[3] = imm;

    PRINT();

    merge(insts, inst);
}

void call(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xe8, 0x00, 0x00, 0x00, 0x00};

    PRINT();

    merge(insts, inst);
}

// 0x99
void cdq(std::ifstream& ifs) {
    std::vector<uint8_t> inst{0x99};

    PRINT();

    merge(insts, inst);
}

// 0x39 0b11<src><dst>
void cmpl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst;

    if(arg1[0] == '$') {
    // cmpl imm, reg

        inst = {0x83, 0xf8, 0x00};

        uint8_t reg = Reg2Int(arg2);

        int imm = Imm2Int(arg1);
        // temp hack for now
        assert(imm == (int8_t)imm);

        inst[1] |= reg;
        inst[2] = imm;

    }
    else {
        inst = {0x39, 0xc0};

        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst[1] |= (src << 3);
        inst[1] |= dst;
    }

    PRINT();

    merge(insts, inst);
}

// 0xf7 0b11'111'<reg>
void idivl(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xf7, 0xf8};

    int reg = Reg2Int(arg);

    inst[1] |= reg;

    PRINT();

    merge(insts, inst);
}

// 0x0f'af 0b11'<dst>'<src>
void imul(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);
    
    std::vector<uint8_t> inst{0x0f, 0xaf, 0xc0};
    uint src = Reg2Int(arg1);
    uint dst = Reg2Int(arg2);

    inst[2] |= src;
    inst[2] |= (dst << 3);

    PRINT();

    merge(insts, inst);
}

void _int(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xcd, 0x00};

    assert(arg[0] == '$');
    arg = arg.substr(1, arg.size()-1);
    uint imm = stoi(arg, nullptr, 16);

    inst[1] = (uint8_t)imm;

    PRINT();

    merge(insts, inst);
}

void je(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x74, 0x00};

    int tag = tagMap[arg];
    int pos = insts.size() + 2;

    int offset = tag - pos;

    assert(offset == (int8_t)offset);

    inst[1] |= (uint8_t)offset;

    PRINT();

    merge(insts, inst);
}

void jmp(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xeb, 0x00};

    int tag = tagMap[arg];
    int pos = insts.size() + 2;

    int offset = tag - pos;

    assert(offset == (int8_t)offset);

    inst[1] |= (uint8_t)offset;

    PRINT();

    merge(insts, inst);
}

void jne(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x75, 0x00};

    int tag = tagMap[arg];
    int pos = insts.size() + 2;

    int offset = tag - pos;

    assert(offset == (int8_t)offset);

    inst[1] |= (uint8_t)offset;

    PRINT();

    merge(insts, inst);
}

//
void movl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst;

    // movl src, dst
    if(arg1[0] == '%' && arg2[0] == '%') {
        inst = {0x89, 0xc0};
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst[1] |= (src << 3);
        inst[1] |= dst;
    }
    // movl imm, reg
    // 0b1011'1<reg> imm
    else if(arg1[0] == '$') {
        inst = {0xb8, 0x00, 0x00, 0x00, 0x00};
        int imm = Imm2Int(arg1);
        uint8_t reg = Reg2Int(arg2);
        char* p = (char*)&imm;

        inst[0] |= reg;
        inst[1] |= p[0];
        inst[2] |= p[1];
        inst[3] |= p[2];
        inst[4] |= p[3];
    }
    // movl src, offset(dst)
    else if(arg1[0] == '%') {
        int offset = stoi(arg2);
        assert((int8_t)offset == offset);

        arg2 = arg2.substr(arg2.size()-5, 4);

        inst = {0x67, 0x89, 0x40, 0x00};
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst[2] |= (src << 3);
        inst[2] |= dst;
        inst[3] |= offset;
    }
    // movl offset(src), dst
    else if(arg2[0] == '%') {
        int offset = stoi(arg1);
        assert((int8_t)offset == offset);

        arg1 = arg1.substr(arg1.size()-5, 4);

        inst = {0x67, 0x8b, 0x40, 0x00};
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst[2] |= (dst << 3);
        inst[2] |= src;
        inst[3] |= offset;
    }
    else
        assert(false);

    PRINT();

    merge(insts, inst);
}

void movq(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst;

    // movl src, dst
    inst = {0x48, 0x89, 0xc0};
    uint src = Reg2Int(arg1);
    uint dst = Reg2Int(arg2);

    inst[2] |= (src << 3);
    inst[2] |= dst;

    PRINT();

    merge(insts, inst);
}

void neg(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xf7, 0xd8};

    int reg = Reg2Int(arg);

    inst[1] |= reg;

    PRINT();

    merge(insts, inst);
}

void _not(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xf7, 0xd0};

    int reg = Reg2Int(arg);

    inst[1] |= reg;

    PRINT();

    merge(insts, inst);
}

// 0x58 + reg
void pop(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x58};

    uint8_t reg = Reg2Int(arg);

    inst[0] += reg;

    PRINT();

    merge(insts, inst);
}

// 0x50 + reg
void push(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x50};

    uint8_t reg = Reg2Int(arg);

    inst[0] += reg;

    PRINT();

    merge(insts, inst);
}

// 0xc3
void ret(std::ifstream& ifs) {
    std::vector<uint8_t> inst{0xc3};

    PRINT();

    merge(insts, inst);
}

void sete(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x0f, 0x94, 0xc0};

    uint8_t reg = Reg2Int(arg);

    inst[2] |= reg;

    PRINT();

    merge(insts, inst);
}

void setg(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x0f, 0x9f, 0xc0};

    uint8_t reg = Reg2Int(arg);

    inst[2] |= reg;

    PRINT();

    merge(insts, inst);
}

void setge(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x0f, 0x9d, 0xc0};

    uint8_t reg = Reg2Int(arg);

    inst[2] |= reg;

    PRINT();

    merge(insts, inst);
}

void setl(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x0f, 0x9c, 0xc0};

    uint8_t reg = Reg2Int(arg);

    inst[2] |= reg;

    PRINT();

    merge(insts, inst);
}

void setle(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x0f, 0x9e, 0xc0};

    uint8_t reg = Reg2Int(arg);

    inst[2] |= reg;

    PRINT();

    merge(insts, inst);
}

void setne(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x0f, 0x95, 0xc0};

    uint8_t reg = Reg2Int(arg);

    inst[2] |= reg;

    PRINT();

    merge(insts, inst);
}

void subl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst{0x29, 0xc0};

    uint8_t src = Reg2Int(arg1);
    uint8_t dst = Reg2Int(arg2);

    inst[1] |= src << 3;
    inst[1] |= dst;

    PRINT();

    merge(insts, inst);
}

void syscall(std::ifstream& ifs) {
    std::vector<uint8_t> inst{0x0f, 0x05};

    PRINT();

    merge(insts, inst);
}

void _xor(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst{0x48, 0x31, 0xc0};

    uint8_t src = Reg2Int(arg1);
    uint8_t dst = Reg2Int(arg2);

    inst[2] |= src << 3;
    inst[2] |= dst;

    PRINT();

    merge(insts, inst);
}

} // namespace

void assemble(std::ifstream& ifs) {
    std::string token;

    while(ifs >> token) {
        if(token == "addl") {
            addl(ifs);
            continue;
        }
        if(token == "addq") {
            addq(ifs);
            continue;
        }
        if(token == "call") {
            call(ifs);
            continue;
        }
        if(token == "cdq") {
            cdq(ifs);
            continue;
        }
        if(token == "cmpl") {
            cmpl(ifs);
            continue;
        }
        if(token == "idivl") {
            idivl(ifs);
            continue;
        }
        if(token == "imul") {
            imul(ifs);
            continue;
        }
        if(token == "je") {
            je(ifs);
            continue;
        }
        if(token == "jmp") {
            jmp(ifs);
            continue;
        }
        if(token == "jne") {
            jne(ifs);
            continue;
        }
        if(token == "movl") {
            movl(ifs);
            continue;
        }
        if(token == "movq") {
            movq(ifs);
            continue;
        }
        if(token == "neg") {
            neg(ifs);
            continue;
        }
        if(token == "not") {
            _not(ifs);
            continue;
        }
        if(token == "pop") {
            pop(ifs);
            continue;
        }
        if(token == "push") {
            push(ifs);
            continue;
        }
        if(token == "ret") {
            ret(ifs);
            continue;
        }
        if(token == "sete") {
            sete(ifs);
            continue;
        }
        if(token == "setg") {
            setg(ifs);
            continue;
        }
        if(token == "setge") {
            setge(ifs);
            continue;
        }
        if(token == "setl") {
            setl(ifs);
            continue;
        }
        if(token == "setle") {
            setle(ifs);
            continue;
        }
        if(token == "setne") {
            setne(ifs);
            continue;
        }
        if(token == "subl") {
            subl(ifs);
            continue;
        }
        if(token == "syscall") {
            syscall(ifs);
            continue;
        }
        if(token == "xor") {
            _xor(ifs);
            continue;
        }

        D("%s\n", token.c_str());
    }
}