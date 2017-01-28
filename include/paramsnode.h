#ifndef PARAMSNODE_H
#define PARAMSNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class ParamsNode : public AstNode {
    public:
        static int count;
        ParamsNode();
        ~ParamsNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        void addParamName(std::string name);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
        std::string mname;
    private:
        int id;
};

#endif