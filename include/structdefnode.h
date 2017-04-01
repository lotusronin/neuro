#ifndef STRUCTDEFNODE_H
#define STRUCTDEFNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class StructDefNode : public AstNode {
    public:
        static int count;
        StructDefNode();
        ~StructDefNode();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::string& getIdent();
        std::vector<AstNode*>* getChildren();
        //std::vector<AstNode*> mchildren;
        std::string ident;
        bool foreign;
    //private:
        int id;
};

#endif //STRUCTDEFNODE_H
