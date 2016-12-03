#ifndef CCODEGEN_H
#define CCODEGEN_H

#include <string>
#include <vector>
#include <fstream>
#include "astnodetypes.h"
#include "symboltable.h"

void genCFile(std::string filename, const std::vector<SymbolTableEntry*>& exported);
void testGenCFile();

#endif //CCODEGEN_H
