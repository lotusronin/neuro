#ifndef STRUCTDEFNODE_H
#define STRUCTDEFNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class StructDefNode : public AstNode {
    public:
        static int count;
        StructDefNode(AstNodeType nodet);
        ~StructDefNode();
        std::string& getIdent();
        std::string ident;
        bool foreign;
        int id;
};

#endif //STRUCTDEFNODE_H
