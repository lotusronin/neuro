#ifndef ASTNODE_H
#define ASTNODE_H

#include <fstream>
#include <vector>
#include "tokens.h"

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
    FuncCall,
    Const,
    Args,
    LoopStmt,
    Assign,
    Default,
    Cast,
    SizeOf
};

enum class SemanticType {
    Bool,
    Int,
    Void,
    Char,
    Double,
    Float,
    Typeless,
    Infer,
    User,
    u8,
    u16,
    u32,
    u64,
    s8,
    s16,
    s32,
    s64,
    intlit,
    floatlit
};

std::ostream& operator<<(std::ostream& os, const SemanticType& obj);

struct TypeInfo {
    const char* userid = nullptr;
    SemanticType type = SemanticType::Typeless;
    int indirection = 0;
};

std::ostream& operator<<(std::ostream& os, const TypeInfo& obj);

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



#endif
