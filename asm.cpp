#include <fstream>
#include <cassert>
#include <iostream>

#ifdef DEBUG
    #define D(...) printf(__VA_ARGS__)
#else
    #define D(...)
#endif

namespace{

inline uint8_t Reg2Int(std::string reg) {
    if(reg == "%eax") return 0;
    if(reg == "%ecx") return 1;
    if(reg == "%edx") return 2;
    if(reg == "%ebx") return 3;
    if(reg == "%esp") return 4;
    if(reg == "%ebp") return 5;
    if(reg == "%esi") return 6;
    if(reg == "%edi") return 7;

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

} // namespace

// 0x50 + reg
void push(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    int reg = Reg2Int(arg);
    D("0x%x\n", 0x50 + reg);
}

// add imm, reg
// 0x83 0b11'000'<reg> imm
// add src, dst
// 0x01 0b11'<src>'<dst>
void addl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    if(arg1[0] == '$') {
    // add imm, reg

        int imm = Imm2Int(arg1);
        // temp hack for now
        assert(imm == (int8_t)imm);

        D("0x83c%x%02x\n", Reg2Int(arg2), imm);

    }
    else {
    // add src, dst
        uint inst = 0x01'c0;
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst |= (src << 3);
        inst |= dst;

        D("0x%04x\n", inst);

    }
}

// 0x99
void cdq(std::ifstream& ifs) {
    D("0x99\n");
}

// 0x39 0b11<src><dst>
void cmpl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    uint inst = 0x39'c0;
    uint src = Reg2Int(arg1);
    uint dst = Reg2Int(arg2);

    inst |= (src << 3);
    inst |= dst;

    D("0x%04x\n", inst);
}

// 0xf7 0b11'111'<reg>
void idivl(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    int reg = Reg2Int(arg);

    uint16_t inst = 0xf7'f8;
    inst |= reg;

    D("0x%04x\n", inst);
}

// 0x0f'af 0b11'<dst>'<src>
void imul(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);
    
    uint inst = 0x0f'af'c0;
    uint src = Reg2Int(arg1);
    uint dst = Reg2Int(arg2);

    inst |= src;
    inst |= (dst << 3);

    D("0x%06x\n", inst);
}

//
void movl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    // movl src, dst
    if(arg1[0] == '%' && arg2[0] == '%') {
        uint inst = 0x89'c0;
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst |= (src << 3);
        inst |= dst;

        D("0x%04x\n", inst);
    }
    // movl imm, reg
    // 0b1011'1<reg> imm
    else if(arg1[0] == '$') {
        uint64_t inst = 0xb8'00000000;
        int imm = Imm2Int(arg1);
        uint64_t reg = Reg2Int(arg2);
        char* p = (char*)&imm;

        inst |= reg << 32;
        inst |= p[0] << 24;
        inst |= p[1] << 16;
        inst |= p[2] << 8;
        inst |= p[3];

        D("%010lx\n", inst);
    }
    // movl src, offset(dst)
    else if(arg1[0] == '%') {
        int offset = stoi(arg2);
        assert((int8_t)offset == offset);

        arg2 = arg2.substr(arg2.size()-5, 4);

        uint inst = 0x89'40'00;
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst |= (src << 11);
        inst |= (dst << 8);
        inst |= offset;

        D("0x%06x\n", inst);
    }
    // movl offset(src), dst
    else if(arg2[0] == '%') {
        int offset = stoi(arg1);
        assert((int8_t)offset == offset);

        arg1 = arg1.substr(arg1.size()-5, 4);

        uint inst = 0x8b'40'00;
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst |= (dst << 11);
        inst |= (src << 8);
        inst |= offset;

        D("0x%06x\n", inst);
    }
    else
        assert(false);
}