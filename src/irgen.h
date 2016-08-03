#ifndef IRGEN_H
#define IRGEN_H

#include "astnode.h"
#include <string>

void generateIR(AstNode* ast);
void dumpIR();
void writeObj(std::string o);
void writeIR(std::string o);

#endif
