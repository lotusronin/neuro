#include <iostream>
#include "node.h"
extern NBlock* programBlock;
extern NProgram neuroprogram;
extern int yyparse();

int main(int argc, char **argv) {
    yyparse();
    std::cout << programBlock << std::endl;
    neuroprogram.print();
    return 0;
}
