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
        std::string getFileName();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        //std::vector<AstNode*> mchildren;
    //private:
        int id;
        std::string mname;
};

#endif
