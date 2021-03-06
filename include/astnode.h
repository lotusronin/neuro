#ifndef ASTNODE_H
#define ASTNODE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "tokens.h"
#include "arrayview.h"
#include "typeinfo.h"

enum class AstNodeType {
    Program,
    CompileUnit,
    TLStmnt,
    ImportStmnt,
    Prototype,
    Params,
    Type,
    Var,
    VarDec,
    VarDecAssign,
    FuncDef,
    StructDef,
    Block,
    Stmt,
    IfStmt,
    ElseStmt,
    ForLoop,
    DeferStmt,
    WhileLoop,
    RetStmnt,
    Expression,
    BinOp,
    UnaryOp,
    FuncCall,
    Const,
    Args,
    LoopStmtBrk,
    LoopStmtCnt,
    Assign,
    Default,
    Cast,
    SizeOf,
    UnionDef
};

class AstNode {
    public:
        AstNode();
        ~AstNode();
        AstNodeType nodeType() const;
        SemanticType getType() const;
        TypeInfo* getTypeInfo();
        void addChild(AstNode* child);
        AstNode* lastChild();
        void setToken(Token& t);
        std::vector<AstNode*>* getChildren();
        Token mtoken;
        TypeInfo mtypeinfo;
        std::vector<AstNode*> mchildren;
        AstNodeType mnodet = AstNodeType::Default;
};


class AssignNode : public AstNode {
    public:
        static int count;
        AssignNode();
        AssignNode(AssignNode* n);
        ~AssignNode();
        void addChild(AstNode* node);
        AstNode* getLHS();
        AstNode* getRHS();
        int id;
};


class BinOpNode : public AstNode {
    public:
        static int count;
        static int constructed;
        BinOpNode(const AstNodeType ntype);
        BinOpNode(BinOpNode* n);
        ~BinOpNode();
        void setOp(const char* op);
        const char* getOp() const;
        AstNode* LHS();
        void setLHS(AstNode* ast);
        AstNode* RHS();
        void setRHS(AstNode* ast);
        void setToken(Token& t);
        SemanticType getType() const;
        static void printDeleted();
        static int deleted;
        const char* mop;
        int id;
        bool unaryOp;
        AstNode* opOverload;
};


class BlockNode : public AstNode {
    public:
        static int count;
        BlockNode();
        ~BlockNode();
        int getId() const;
        int id;
};


class CastNode : public AstNode {
    public:
        static int count;
        CastNode();
        CastNode(CastNode* n);
        ~CastNode();
        void addChild(AstNode* node);
        TypeInfo fromType;
        int id;
};


class CompileUnitNode : public AstNode {
    public:
        static int count;
        CompileUnitNode();
        ~CompileUnitNode();
        void setFileName(const std::string& name);
        std::string getFileName() const;
        int id;
        std::string mname;
        std::vector<std::string> imports;
};


class ConstantNode : public AstNode {
    public:
        static int count;
        ConstantNode();
        ConstantNode(ConstantNode* n);
        ~ConstantNode();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        void setVal(const char* val);
        void setToken(const Token& t);
        const char* getVal() const;
        int id;
        const char* mval;
};


class DeferStmtNode : public AstNode {
    public:
        static int count;
        DeferStmtNode();
        ~DeferStmtNode();
        int id;
};


class FuncDefNode : public AstNode {
    public:
        static int count;
        FuncDefNode(const AstNodeType ntype, const std::string& funcname);
        FuncDefNode(FuncDefNode* n);
        ~FuncDefNode();
        void addFuncName(const std::string& funcname);
        ArrayView getParameters();
        AstNode* getFunctionBody();
        std::string mangledName();
        std::string mfuncname;
        char* op;
        int isOperatorOverload;
        int isTemplated;
        int id;
};


class FuncCallNode : public AstNode {
    public:
        static int count;
        FuncCallNode(const std::string& funcname);
        FuncCallNode(FuncCallNode* n);
        ~FuncCallNode();
        void addFuncName(const std::string& funcname);
        int id;
        std::string mfuncname;
        std::string scopes;
        FuncDefNode* func;
        std::unordered_map<std::string,TypeInfo> templateTypeParameters;
        bool specialized;
};


class IfNode : public AstNode {
    public:
        static int count;
        IfNode();
        IfNode(IfNode* n);
        ~IfNode();
        AstNode* getConditional();
        AstNode* getThen();
        AstNode* getElse();
        int id;
};


class LoopNode : public AstNode {
    public:
        static int count;
        LoopNode(const AstNodeType ntype);
        LoopNode(LoopNode* n);
        ~LoopNode();
        AstNode* getConditional();
        AstNode* getUpdate();
        AstNode* getInit();
        AstNode* getBody();
        int getId() const ;
        int id;
};


class LoopStmtNode : public AstNode {
    public:
        static int count;
        LoopStmtNode();
        LoopStmtNode(LoopStmtNode* n);
        ~LoopStmtNode();
        int id;
};


class ParamsNode : public AstNode {
    public:
        static int count;
        ParamsNode(const std::string& name);
        ParamsNode(ParamsNode* n);
        ~ParamsNode();
        void addParamName(std::string& name);
        std::string mname;
        int id;
};


class ProgramNode : public AstNode {
    public:
        ProgramNode();
        ~ProgramNode();
};


class ReturnNode : public AstNode {
    public:
        static int count;
        ReturnNode();
        ReturnNode(ReturnNode* n);
        ~ReturnNode();
        int id;
};


class SizeOfNode : public AstNode {
    public:
        static int count;
        SizeOfNode();
        ~SizeOfNode();
        int id;
};


class StructDefNode : public AstNode {
    public:
        static int count;
        StructDefNode(const AstNodeType nodet);
        ~StructDefNode();
        std::string& getIdent();
        std::string ident;
        bool foreign;
        int id;
        int isTemplated;
        std::unordered_set<std::string> templateTypeNames;
};


class VarDeclNode : public AstNode {
    public:
        static int count;
        VarDeclNode(const AstNodeType ntype);
        VarDeclNode(VarDeclNode* n);
        ~VarDeclNode();
        AstNode* getLHS();
        AstNode* getRHS();
        int id;
};


class VarNode : public AstNode {
    public:
        static int count;
        VarNode(const std::string& name);
        VarNode(VarNode* n);
        ~VarNode();
        void addVarName(const std::string& name);
        const char* getVarName() const;
        int id;
        std::string mname;
};

#endif
