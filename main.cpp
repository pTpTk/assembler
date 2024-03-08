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

        std::cout << token << std::endl;
    }

}