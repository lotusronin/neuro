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
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::string& getIdent();
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
        std::string ident;
    private:
        int id;
};

#endif //STRUCTDEFNODE_H
