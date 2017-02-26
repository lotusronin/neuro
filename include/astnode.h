#ifndef ASTNODE_H
#define ASTNODE_H

#include <fstream>
#include <vector>
#include "tokens.h"

/*
void parseTopLevelStatements(LexerTarget* lexer);
void parseImportStatement(LexerTarget* lexer);
void parsePrototype(LexerTarget* lexer);
void parseOptparams(LexerTarget* lexer);
void parseType(LexerTarget* lexer);
void parseVar(LexerTarget* lexer);
void parseVarDec(LexerTarget* lexer);
void parseVarDecAssign(LexerTarget* lexer);
void parseFunctionDef(LexerTarget* lexer);
void parseBlock(LexerTarget* lexer);
void parseStatementList(LexerTarget* lexer);
void parseStatement(LexerTarget* lexer);
void parseStatementListLoop(LexerTarget* lexer);
void parseIfblock(LexerTarget* lexer);
void parseIfElseBody(LexerTarget* lexer);
void parseOptElseBlock(LexerTarget* lexer);
void parseLoop(LexerTarget* lexer);
void parseForLoop(LexerTarget* lexer);
void parseDeferBlock(LexerTarget* lexer);
void parseWhileLoop(LexerTarget* lexer);
void parseReturnStatement(LexerTarget* lexer);
void parseExpression(LexerTarget* lexer);
void parseMultdiv(LexerTarget* lexer);
void parseParenexp(LexerTarget* lexer);
void parseConst(LexerTarget* lexer);
void parseFunccall(LexerTarget* lexer);
void parseOptargs(LexerTarget* lexer);
*/
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
    Cast
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
    SemanticType type = SemanticType::Typeless;
    std::string userid;
    int indirection = 0;
};

std::ostream& operator<<(std::ostream& os, const TypeInfo& obj);

class AstNode {
    public:
        AstNode();
        virtual ~AstNode();
        virtual AstNodeType nodeType();
        SemanticType getType();
        TypeInfo* getTypeInfo();
        void addChild(AstNode* child);
        virtual AstNode* lastChild();
        virtual void setToken(Token& t);
        std::vector<AstNode*>* getChildren();
        Token mtoken;
        SemanticType mstype = SemanticType::Typeless;
        TypeInfo mtypeinfo;
        std::vector<AstNode*> mchildren;
};



#endif
