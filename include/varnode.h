#ifndef VARNODE_H
#define VARNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class VarNode : public AstNode {
    public:
        static int count;
        VarNode();
        ~VarNode();
        void addVarName(std::string name);
        const char* getVarName() const;
        int id;
        std::string mname;
};

#endif
