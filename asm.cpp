#include <fstream>
#include <cassert>

inline uint Reg2Int(std::string reg) {
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

void push(std::ifstream& ifs) {
    std::string reg;
    ifs >> reg;
    assert(reg[0] == '%');
    printf("0x%x\n", 0x50 + Reg2Int(reg));
}

void addl(std::ifstream& ifs) {
    std::string arg1, arg2;
    ifs >> arg1 >> arg2;

    // get rid of ','
    arg1.resize(arg1.size()-1);

    if(arg1[0] == '$') {
    // add imm, reg

        int imm = Imm2Int(arg1);
        // temp hack for now
        assert(imm == (int8_t)imm);

        printf("0x83c%x%02x\n", Reg2Int(arg2), imm);

    }
    else {
    // add src, dst
        uint inst = 0x01'c0;
        uint src = Reg2Int(arg1);
        uint dst = Reg2Int(arg2);

        printf("src = %d, dst = %d\n", src, dst);

        inst |= (src << 3);
        inst |= dst;

        printf("0x%04x\n", inst);

    }
}