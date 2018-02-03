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
    UnaryOp,
    FuncCall,
    Const,
    Args,
    LoopStmt,
    Assign,
    Default,
    Cast,
    SizeOf,
    UnionDef
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
    floatlit,
    nulllit,
    Array
};

std::ostream& operator<<(std::ostream& os, const SemanticType& obj);

struct TypeInfo {
    const char* userid = nullptr;
    TypeInfo* base_t = nullptr;
    SemanticType type = SemanticType::Typeless;
    int pindirection = 0;
    int arr_size = 0;
    int indirection() const;
    bool isPointer() const;
    bool isArray() const;
    int arraySize() const;
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
