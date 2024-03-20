#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

#include "asm.cpp"
// #include "elf.hpp"

int main(int argc, char** argv) {
    assert(argc == 2);

    std::ifstream ifs1(argv[1]);
    std::ifstream ifs2(argv[1]);
    std::string token;
    collectTags(ifs1);

    while(ifs2 >> token) {
        if(token == "addl") {
            addl(ifs2);
            continue;
        }
        if(token == "cdq") {
            cdq(ifs2);
            continue;
        }
        if(token == "cmpl") {
            cmpl(ifs2);
            continue;
        }
        if(token == "idivl") {
            idivl(ifs2);
            continue;
        }
        if(token == "imul") {
            imul(ifs2);
            continue;
        }
        if(token == "je") {
            je(ifs2);
            continue;
        }
        if(token == "jmp") {
            jmp(ifs2);
            continue;
        }
        if(token == "jne") {
            jne(ifs2);
            continue;
        }
        if(token == "movl") {
            movl(ifs2);
            continue;
        }
        if(token == "neg") {
            neg(ifs2);
            continue;
        }
        if(token == "not") {
            _not(ifs2);
            continue;
        }
        if(token == "pop") {
            pop(ifs2);
            continue;
        }
        if(token == "push") {
            push(ifs2);
            continue;
        }
        if(token == "ret") {
            ret(ifs2);
            continue;
        }
        if(token == "sete") {
            sete(ifs2);
            continue;
        }
        if(token == "setg") {
            setg(ifs2);
            continue;
        }
        if(token == "setge") {
            setge(ifs2);
            continue;
        }
        if(token == "setl") {
            setl(ifs2);
            continue;
        }
        if(token == "setle") {
            setle(ifs2);
            continue;
        }
        if(token == "setne") {
            setne(ifs2);
            continue;
        }
        if(token == "subl") {
            subl(ifs2);
            continue;
        }

        std::cout << token << std::endl;
    }

    for(auto c : shstrtab) {
        printf("%c", c);
    }
    printf("\n");

}