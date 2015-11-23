#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node {
    public:
        static int nid;
        int unique_id;
        Node();
        virtual ~Node();
        //virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
};

class NExpression : public Node {
    public:
    NExpression();
};

class NStatement : public Node {
    public:
    NStatement();
    virtual void print();
};

class NInteger : public NExpression {
    public:
        long long value;
        NInteger(long long value);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
    public:
        double value;
        NDouble(double value);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
    public:
        std::string name;
        NIdentifier(const std::string& name);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
    public:
        const NIdentifier& id;
        ExpressionList arguments;
        NMethodCall(const NIdentifier& id, ExpressionList& arguments);
        NMethodCall(const NIdentifier& id);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
    public:
        int op;
        NExpression& lhs;
        NExpression& rhs;
        NBinaryOperator(NExpression& lhs, int op, NExpression& rhs);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
    public:
        NIdentifier& lhs;
        NExpression& rhs;
        NAssignment(NIdentifier& lhs, NExpression& rhs);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
    public:
        StatementList statements;
        NBlock();
        //virtual llvm::Value* codeGen(CodeGenContext& context);
       void print(); 
};

class NExpressionStatement : public NStatement {
    public:
        NExpression& expression;
        NExpressionStatement(NExpression& expression);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
    public:
            const NIdentifier& type;
            NIdentifier& id;
            NExpression *assignmentExpr;
            NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
                                    type(type), id(id) { }
            NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
                                        type(type), id(id), assignmentExpr(assignmentExpr) { }
            //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
    public:
        const NIdentifier& type;
        const NIdentifier& id;
        VariableList arguments;
        NBlock& block;
        NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, const VariableList& arguments, NBlock& block);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionPrototype : public NStatement {
    public:
        const NIdentifier& type;
        const NIdentifier& id;
        VariableList arguments;
        NFunctionPrototype(const NIdentifier& type, const NIdentifier& id, const VariableList& arguments);
        //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NProgram : public Node {
    public:
        std::vector<NFunctionPrototype*> prototypes;
        std::vector<NBlock*> blocks;
        NProgram();
        void print();
};

#endif
