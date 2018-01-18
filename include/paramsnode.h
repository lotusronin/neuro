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
        void addParamName(std::string name);
        std::string mname;
        int id;
};

#endif
