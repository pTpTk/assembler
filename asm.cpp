#include <fstream>
#include <cassert>
#include <iostream>
#include <vector>

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
}

// 0x99
void cdq(std::ifstream& ifs) {
    std::vector<uint8_t> inst{0x99};

    PRINT();
}

// 0x39 0b11<src><dst>
void cmpl(std::ifstream& ifs) {
    std::string arg1, arg2;
    Get2Args(ifs, arg1, arg2);

    std::vector<uint8_t> inst{0x39, 0xc0};

    uint src = Reg2Int(arg1);
    uint dst = Reg2Int(arg2);

    inst[1] |= (src << 3);
    inst[1] |= dst;

    PRINT();
}

// 0xf7 0b11'111'<reg>
void idivl(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xf7, 0xf8};

    int reg = Reg2Int(arg);

    inst[1] |= reg;

    PRINT();
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

        inst = {0x89, 0x40, 0x00};
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst[1] |= (src << 3);
        inst[1] |= dst;
        inst[2] |= offset;
    }
    // movl offset(src), dst
    else if(arg2[0] == '%') {
        int offset = stoi(arg1);
        assert((int8_t)offset == offset);

        arg1 = arg1.substr(arg1.size()-5, 4);

        inst = {0x8b, 0x40, 0x00};
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        inst[1] |= (dst << 3);
        inst[1] |= src;
        inst[2] |= offset;
    }
    else
        assert(false);

    PRINT();
}

void neg(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xf7, 0xd8};

    int reg = Reg2Int(arg);

    inst[1] |= reg;

    PRINT();
}

void _not(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0xf7, 0xd0};

    int reg = Reg2Int(arg);

    inst[1] |= reg;

    PRINT();
}

// 0x58 + reg
void pop(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x58};

    uint8_t reg = Reg2Int(arg);

    inst[0] += reg;

    PRINT();
}

// 0x50 + reg
void push(std::ifstream& ifs) {
    std::string arg;
    ifs >> arg;

    std::vector<uint8_t> inst{0x50};

    uint8_t reg = Reg2Int(arg);

    inst[0] += reg;

    PRINT();
}

// 0xc3
void ret(std::ifstream& ifs) {
    std::vector<uint8_t> inst{0x99};

    PRINT();
}