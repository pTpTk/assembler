#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

#include "asm.cpp"

int main(int argc, char** argv) {
    assert(argc == 2);

    std::ifstream ifs(argv[1]);
    std::string token;
    while(ifs >> token) {
        if(token == "push") {
            push(ifs);
            continue;
        }
        if(token == "addl") {
            addl(ifs);
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
        if(token == "movl") {
            movl(ifs);
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

        std::cout << token << std::endl;
    }

}