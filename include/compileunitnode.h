#ifndef COMPILEUNITNODE_H
#define COMPILEUNITNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class CompileUnitNode : public AstNode {
    public:
        static int count;
        CompileUnitNode();
        ~CompileUnitNode();
        void setFileName(std::string name);
        std::string getFileName() const;
        int id;
        std::string mname;
        std::vector<std::string> imports;
};

#endif
