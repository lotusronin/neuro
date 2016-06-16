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
    Default
};

enum class SemanticType {
    Bool,
    Int,
    Void,
    Char,
    Double,
    Float,
    Typeless,
    Infer
};

std::ostream& operator<<(std::ostream& os, const SemanticType& obj);

class AstNode {
    public:
        AstNode();
        virtual ~AstNode();
        virtual void makeGraph(std::ofstream& outfile);
        virtual AstNodeType nodeType();
        virtual SemanticType getType();
        virtual void addChild(AstNode* child);
        virtual AstNode* lastChild();
        virtual void setToken(Token& t);
        virtual std::vector<AstNode*>* getChildren();
        Token mtoken;
        SemanticType mstype = SemanticType::Typeless;
};



#endif
