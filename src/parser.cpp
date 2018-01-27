#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <assert.h>
#include "parser.h"
#include "tokens.h"
#include "astnodetypes.h"
#include <string.h>

//ParseErrorType:: is a pain to write out every time... :)
#define PET ParseErrorType

//TODO(marcus): Currently this list is global. Is this okay or should we do it differently?
//Global imported files hashmap
std::unordered_map<std::string, CompileUnitNode*> importedFiles;

void parseTopLevelStatements(LexerTarget* lexer, AstNode* parent);
void parseImportStatement(LexerTarget* lexer, AstNode* parent);
void parsePrototype(LexerTarget* lexer, CompileUnitNode* parent);
void parseStructDef(LexerTarget* lexer, AstNode* parent);
void parseStructDefBody(LexerTarget* lexer, AstNode* parent);
void parseOptparams(LexerTarget* lexer, AstNode* parent);
void parseType(LexerTarget* lexer, AstNode* parent);
void parseVar(LexerTarget* lexer, AstNode* parent);
void parseVarAssign(LexerTarget* lexer, AstNode* parent);
void parseSomeVarDecStmt(LexerTarget* lexer, AstNode* parent);
void parseFunctionDef(LexerTarget* lexer, AstNode* parent);
void parseBlock(LexerTarget* lexer, AstNode* parent);
void parseStatementList(LexerTarget* lexer, AstNode* parent);
void parseStatement(LexerTarget* lexer, AstNode* parent);
void parseIfblock(LexerTarget* lexer, AstNode* parent);
void parseLoop(LexerTarget* lexer, AstNode* parent);
void parseForLoop(LexerTarget* lexer, AstNode* parent);
void parseDeferBlock(LexerTarget* lexer, AstNode* parent);
void parseWhileLoop(LexerTarget* lexer, AstNode* parent);
void parseReturnStatement(LexerTarget* lexer, AstNode* parent);
void parseExpression(LexerTarget* lexer, AstNode* parent);
AstNode* parseLogicalOr(LexerTarget* lexer);
AstNode* parseLogicalAnd(LexerTarget* lexer);
AstNode* parseBitOr(LexerTarget* lexer);
AstNode* parseBitXor(LexerTarget* lexer);
AstNode* parseEqneq(LexerTarget* lexer);
AstNode* parseGLTE(LexerTarget* lexer);
AstNode* parsePlusmin(LexerTarget* lexer);
AstNode* parseMultdiv(LexerTarget* lexer);
AstNode* parseAddrOfIndir(LexerTarget* lexer);
AstNode* parseMemberAccess(LexerTarget* lexer);
AstNode* parseParenexp(LexerTarget* lexer);
void parseConst(LexerTarget* lexer, AstNode* parent);
void parseFunccallOrVar(LexerTarget* lexer, AstNode* parent);
void parseFunccall(LexerTarget* lexer, AstNode* parent);
void parseOptargs(LexerTarget* lexer, AstNode* parent);
void parseLoopStmt(LexerTarget* lexer, AstNode* parent);
void parsePrototypeOrStruct(LexerTarget* lexer, CompileUnitNode* parent);
AstNode* parseExpression(LexerTarget* lexer, int precedence);

bool fileImported(std::string f) {
    auto iter = importedFiles.find(f);
    return (iter != importedFiles.end());
}

CompileUnitNode* importFile(std::string f) {
    CompileUnitNode* compunit = new CompileUnitNode();
    compunit->setFileName(f);
    importedFiles.insert(std::make_pair(f,compunit));
    return compunit;
}

Parser::Parser(LexerTarget* _lexer) {
    mlexer = _lexer;
}

Parser::~Parser() {
}

void Parser::setLexer(LexerTarget* _lexer) {
    mlexer = _lexer;
}

//TODO(marcus): Currently goal program node is global. Is this okay or should we do it differently?
ProgramNode* program;

void printSizes() {
    std::cout << "int " << sizeof(int) << '\n';
    std::cout << "const char* " << sizeof(const char*) << '\n';
    std::cout << "semantic_type " << sizeof(SemanticType) << '\n';
    std::cout << "string " << sizeof(std::string) << '\n';
    std::cout << "token " << sizeof(Token) << '\n';
    std::cout << "typeinfo " << sizeof(TypeInfo) << '\n';
    std::cout << "astnode " << sizeof(AstNode) << '\n';
    std::cout << "program " << sizeof(ProgramNode) << '\n';
    std::cout << "compileunit " << sizeof(CompileUnitNode) << '\n';
    std::cout << "funcdef " << sizeof(FuncDefNode) << '\n';
    std::cout << "params " << sizeof(ParamsNode) << '\n';
    std::cout << "block " << sizeof(BlockNode) << '\n';
    std::cout << "vdecl " << sizeof(VarDeclNode) << '\n';
    std::cout << "loop " << sizeof(LoopNode) << '\n';
    std::cout << "return " << sizeof(ReturnNode) << '\n';
    std::cout << "defer " << sizeof(DeferStmtNode) << '\n';
    std::cout << "loopstmt " << sizeof(LoopStmtNode) << '\n';
    std::cout << "if " << sizeof(IfNode) << '\n';
    std::cout << "funccall " << sizeof(FuncCallNode) << '\n';
    std::cout << "var " << sizeof(VarNode) << '\n';
    std::cout << "binop " << sizeof(BinOpNode) << '\n';
    std::cout << "constant " << sizeof(ConstantNode) << '\n';
    std::cout << "sizeof " << sizeof(SizeOfNode) << '\n';
    std::cout << "assign " << sizeof(AssignNode) << '\n';
    std::cout << "structdef " << sizeof(StructDefNode) << '\n';
    std::cout << "cast " << sizeof(CastNode) << '\n';
    std::cout << "vec<astnode*> " << sizeof(std::vector<AstNode*>) << '\n';
}


AstNode* Parser::parse() {
    //printSizes();
    program = new ProgramNode();
    CompileUnitNode* compunit = importFile(mlexer->targetName());
    program->addChild(compunit);
    //std::cout << "Beginning parse!\n";
    parseTopLevelStatements(mlexer, compunit);

    return program;
}

void parseTopLevelStatements(LexerTarget* lexer, AstNode* parent) {
    //tl_statements -> imports tl_statements 
    //tl_statements -> prototypes tl_statements
    //tl_statements -> functiondefs tl_statements 
    //tl_statements -> structdefs tl_statements 
    //tl_statements -> null
    while(1) {
        Token tok = lexer->peek();
        if(tok.type == TokenType::import) {
            ////std::cout << "import token, beginning to match import statement...\n";
            parseImportStatement(lexer, parent);
            ////std::cout << "import statement matched\n";
        } else if(tok.type == TokenType::fn) {
            //std::cout << "Function Definitions Parse\n";
            parseFunctionDef(lexer, parent);
            //std::cout << "Function Definitions matched\n";
        } else if(tok.type == TokenType::foreign) {
            //std::cout << "extern token, beginning to match prototype...\n";
            parsePrototypeOrStruct(lexer, static_cast<CompileUnitNode*>(parent));
            ////std::cout << "prototype matched\n";
        } else if(tok.type == TokenType::tstruct) {
            parseStructDef(lexer, parent);
        } else if(tok.type == TokenType::eof) {
            //std::cout << "File is parsed, no errors detected!\n\n";
            return;
        }else {
            parse_error(ParseErrorType::BadTopLevelStatement, tok, lexer);
        }
    }
}

void parseImportStatement(LexerTarget* lexer, AstNode* parent) {
    //imports -> . import id ;
    //consume import
    Token tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadImportName, tok, lexer);
    }
    std::string newfilename = std::string(tok.token)+".nro";
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::semicolon) {
        parse_error(ParseErrorType::MissSemicolon, tok, lexer);
    }
    //consume ;
    lexer->lex();

    if(!fileImported(newfilename)) {
        static_cast<CompileUnitNode*>(parent)->imports.push_back(newfilename);
        CompileUnitNode* compunit = importFile(newfilename);
        LexerTarget importlex = LexerTarget(newfilename, lexer->isDebug());
        importlex.lexFile();
        //std::cout << "\nImporting file: " << newfilename << "\n";
        parseTopLevelStatements(&importlex, compunit);
        program->addChild(compunit);
    } else {
        //std::cout << "Already imported " << newfilename << "\n";
    }
    return;
}

void parsePrototypeOrStruct(LexerTarget* lexer, CompileUnitNode* parent) {
    Token tok = lexer->peekNext();
    if(tok.type == TokenType::fn) {
        parsePrototype(lexer, parent);
    } else if(tok.type == TokenType::tstruct) {
        //consume extern;
        lexer->lex();
        parseStructDef(lexer, parent);
        static_cast<StructDefNode*>(parent->mchildren.back())->foreign = true;
        std::cout << "opaque struct found!\n";
    } else {
        parse_error(PET::MissForeign, tok, lexer);
    }
}

void parsePrototype(LexerTarget* lexer, CompileUnitNode* parent) {
    //prototypes -> . extern fn id ( opt_params ) : type ;
    FuncDefNode* protonode = new FuncDefNode(AstNodeType::Prototype);
    //consume extern
    Token tok = lexer->lex();
    if(tok.type != TokenType::fn) {
        parse_error(PET::MissPrototypeFn, tok, lexer);
    }
    //consume fn
    tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadPrototypeName, tok, lexer);
    }
    protonode->addFuncName(tok.token);
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(ParseErrorType::MissLParen, tok, lexer);
    }
    //consume (
    lexer->lex();
    parseOptparams(lexer, protonode);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissRParen, tok, lexer);
    }
    //consume )
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissColon, tok, lexer);
    }
    //consume :
    lexer->lex();
    parseType(lexer, protonode);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissSemicolon, tok, lexer);
    }
    //consume ;
    tok = lexer->lex();
    parent->addChild(protonode);
}

void parseOptparams(LexerTarget* lexer, AstNode* parent) {
    //opt_params -> null | id : type | id : type , opt_params2
    Token tok = lexer->peek();
    if(tok.type == TokenType::rparen) {
        return;
    }
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadFunctionParameter, tok, lexer);
    }
    std::string n = tok.token;
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissColon, tok, lexer);
    }
    ParamsNode* param = new ParamsNode();
    parent->addChild(param);
    param->addParamName(n);
    //consume :
    lexer->lex();
    parseType(lexer, param);
    tok = lexer->peek();
    if(tok.type == TokenType::comma) {
        //consume ,
        lexer->lex();
        //opt_params2 -> id : type | id : type , opt_params2
        while(1) {
            Token tok = lexer->peek();
            if(tok.type != TokenType::id) {
                parse_error(PET::BadOptparamTail, tok, lexer);
            }
            std::string n = tok.token;
            //consume id
            tok = lexer->lex();
            if(tok.type != TokenType::colon) {
                parse_error(PET::Unknown, tok, lexer);
            }
            //consume :
            lexer->lex();
            ParamsNode* param = new ParamsNode();
            parent->addChild(param);
            param->addParamName(n);
            parseType(lexer, param);
            tok = lexer->peek();
            if(tok.type != TokenType::comma) {
                break;
            }
            //consume ,
            lexer->lex();
        }
    }
}

static bool isTokenAType(TokenType t) {
    //Get rid of the if-else-if flow in parseType
    //TODO(marcus): may want to combine this with tokenTypeIsAType()
    switch(t) {
        case TokenType::tint:
        case TokenType::tuint:
        case TokenType::tchar:
        case TokenType::tuchar:
        case TokenType::tushort:
        case TokenType::tshort:
        case TokenType::tlongint:
        case TokenType::tulongint:
        case TokenType::tfloat:
        case TokenType::tdouble:
        case TokenType::tbool:
        case TokenType::id:
        case TokenType::tvoid:
            return true;
        default:
            return false;
    }
}

void parseType(LexerTarget* lexer, AstNode* parent) {
    //type -> int | char | float | double | bool | id
    //optional *'s infront of each type name
    Token tok = lexer->peek();
    TypeInfo t;

    //Handle pointer types
    int indirection = 0;
    while(tok.type == TokenType::star) {
        indirection++;
        //consume *
        lexer->lex();
        tok = lexer->peek();
    }
    t.indirection = indirection;
    /**/
    SemanticType mstype;
    switch(tok.type) {
        case TokenType::tuchar:
        case TokenType::tchar:
            mstype = SemanticType::Char;
            break;
        case TokenType::tint:
            mstype =SemanticType::Int;
            break;
        case TokenType::tbool:
            mstype = SemanticType::Bool;
            break;
        case TokenType::tfloat:
            mstype = SemanticType::Float;
            break;
        case TokenType::tdouble:
            mstype = SemanticType::Double;
            break;
        case TokenType::tvoid:
            mstype = SemanticType::Void;
            break;
        case TokenType::id:
            mstype = SemanticType::User;
            t.userid = tok.token;
            break;
        case TokenType::tuint:
            mstype = SemanticType::u32;
            break;
        case TokenType::tushort:
            mstype = SemanticType::u16;
            break;
        case TokenType::tshort:
            mstype = SemanticType::s16;
            break;
        case TokenType::tlongint:
            mstype = SemanticType::s64;
            break;
        case TokenType::tulongint:
            mstype = SemanticType::u64;
            break;
        default:
            mstype = SemanticType::Typeless;
            break;
    }
    /**/
    t.type = mstype;
    parent->mtypeinfo = t;
    //parent->mstype = t.type;
    if(isTokenAType(tok.type)) {
        //consume int/char/bool/float/double/void/id
        lexer->lex();
    } else {
        parse_error(PET::BadTypeIdentifier, tok, lexer);
    }
    
    return;
}

bool isAssignmentOp(TokenType t) {
    switch(t) {
        case TokenType::assignment:
        case TokenType::addassign:
        case TokenType::subassign:
        case TokenType::mulassign:
        case TokenType::divassign:
            return true;
        default:
            return false;
    }
    return false;
}

/*
 *varstmt -> vardec | vardecassign | varassign
 *varassign -> var = expression
 *vardecassign -> vardec = expression
 */
void parseVar(LexerTarget* lexer, AstNode* parent) {
    //var -> id
    Token tok = lexer->peek();
    if(tok.type != TokenType::id) {
        parse_error(PET::BadVarName, tok, lexer);
    }
    //consume id
    lexer->lex();
    return;
}

void parseVarAssign(LexerTarget* lexer, AstNode* parent) {
    //varassign -> var = expression
    AssignNode* anode = new AssignNode();
    parent->addChild(anode);
    Token tok = lexer->peek();
    if(tok.type != TokenType::id) {
        parse_error(PET::Unknown, tok, lexer);
    }
    VarNode* vnode = new VarNode();
    vnode->addVarName(tok.token);
    anode->addChild(vnode);
    tok = lexer->lex();
    if(!isAssignmentOp(tok.type)) {
        parse_error(PET::MissEqVarDecAssign, tok, lexer);
    }
    anode->mtoken = tok;
    //consume =
    lexer->lex();
    parseExpression(lexer, anode);
    return;
}

bool tokenTypeIsAType(TokenType t) {
    switch (t) {
        case TokenType::tint:
        case TokenType::tuint:
        case TokenType::tchar:
        case TokenType::tuchar:
        case TokenType::tbool:
        case TokenType::tfloat:
        case TokenType::tdouble:
        case TokenType::id:
            return true;
            break;
        case TokenType::star:
            //TODO(marcus): this could also be a syntax error.
            //Or it could be a pointer.
            return true;
            break;
        default:
            return false;
            break;
  }
}

void parseOptType(LexerTarget* lexer, AstNode* parent) {
    Token t = lexer->peek();
    if(tokenTypeIsAType(t.type)) {
        parseType(lexer,parent);
    }
    return;
}

void parseSomeVarDecStmt(LexerTarget* lexer, AstNode* parent) {
    //std::cout << "parsing some sort of var declaration.\n";
    Token tokid = lexer->peek();
    //consume id
    lexer->lex();
    //consume :
    lexer->lex();
    VarNode* vnode = new VarNode();
    vnode->mtoken = tokid;
    vnode->addVarName(tokid.token);

    parseOptType(lexer,vnode);
    Token tok = lexer->peek();

    //Token nextTok = lexer->peekNext();
    // id : . type = expression
    // it : . type
    if(tok.type == TokenType::assignment) {
        //std::cout << "var declaration is assignment, type inferred.\n";
        //std::cout << "current: " << tok.token << " next: " << nextTok.token << "\n";
        // id : . = expression
        //we have type inferenced declaration assignment
        VarDeclNode* vdecassignnode = new VarDeclNode(AstNodeType::VarDecAssign);
        vdecassignnode->mtoken = tok;
        vdecassignnode->addChild(vnode);
        parent->addChild(vdecassignnode);
        //consume =
        lexer->lex();
        parseExpression(lexer, vdecassignnode);
    } else if(tok.type == TokenType::semicolon) {
            //std::cout << "var declaration only\n";
            //std::cout << "current: " << tok.token << " next: " << nextTok.token << "\n";
            //we have a declaration
            VarDeclNode* vdecnode = new VarDeclNode(AstNodeType::VarDec);
            vdecnode->addChild(vnode);
            parent->addChild(vdecnode);
    } else {
        parse_error(PET::MissColon, tok, lexer);
    }
}

bool tokenIsOperator(Token& t) {
    //TODO(marcus): we will want to allow other operators later
    switch(t.type) {
        case TokenType::plus:
        case TokenType::minus:
        case TokenType::star:
        case TokenType::fslash:
            return true;
            break;
        default:
            return false;
    }
}

void parseFunctionDef(LexerTarget* lexer, AstNode* parent) {
    //functiondefs -> . fn id ( opt_params ) : type block
    //consume fn
    FuncDefNode* funcnode = new FuncDefNode(AstNodeType::FuncDef);
    parent->addChild(funcnode);
    Token tok = lexer->lex();
    //std::cout << tok.token << '\n';
    if(tok.type != TokenType::id) {
        parse_error(PET::BadPrototypeName, tok, lexer);
    }

    const char* operatorOverload = "op";
    if(strcmp(operatorOverload, tok.token) == 0) {
        auto next_tok = lexer->peekNext();
        if(tokenIsOperator(next_tok)) {
            //we have an operator overload
            funcnode->isOperatorOverload = 1;
            funcnode->op = (char*) next_tok.token;
            lexer->lex();
        }
    }
    funcnode->mtoken = tok;
    funcnode->addFuncName(tok.token);
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissLParen,tok, lexer);
    }
    //consume (
    lexer->lex();
    parseOptparams(lexer, funcnode);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissRParen,tok, lexer);
    }
    //consume )
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissColon,tok, lexer);
    }
    //consume :
    lexer->lex();
    parseType(lexer, funcnode);
    parseBlock(lexer, funcnode);
}

void parseBlock(LexerTarget* lexer, AstNode* parent) {
    //block -> . { stmtlist }
    BlockNode* blknode = new BlockNode();
    parent->addChild(blknode);
    Token tok = lexer->peek();
    if(tok.type != TokenType::lbrace) {
        parse_error(PET::BadBlockStart, tok, lexer);
    }
    //consume {
    lexer->lex();
    parseStatementList(lexer, blknode);
    tok = lexer->peek();
    if(tok.type == TokenType::eof) {
        parse_error(PET::IncompleteBlock,tok,lexer);
    }
    //consume }
    lexer->lex();
}

void parseStatementList(LexerTarget* lexer, AstNode* parent) {
    //stmtlist -> stmt stmtlisttail
    //stmtlisttail -> stmtlist | null
    while(1) {
        parseStatement(lexer, parent);
        Token tok = lexer->peek();
        if(tok.type == TokenType::rbrace) {
            return;
        }
    }
}

void parseStatement(LexerTarget* lexer, AstNode* parent) {
    //stmt -> deferblock | ifblock | loop | block | returnstmt
    //     -> varstmt ; | expr ; | ;
    Token tok = lexer->peek();
    if(tok.type == TokenType::semicolon) {
        //consume ;
        lexer->lex();
        return;
    } else if(tok.type == TokenType::sif) {
        parseIfblock(lexer, parent);
    } else if(tok.type == TokenType::sfor || tok.type == TokenType::swhile) {
        parseLoop(lexer, parent);
    } else if(tok.type == TokenType::sdefer) {
        parseDeferBlock(lexer, parent);
    } else if(tok.type == TokenType::lbrace) {
        parseBlock(lexer, parent);
    } else if(tok.type == TokenType::sreturn) {
        parseReturnStatement(lexer, parent);
    } else if(tok.type == TokenType::sbreak || tok.type == TokenType::scontinue) {
        parseLoopStmt(lexer, parent);
    } else if(tok.type == TokenType::id && lexer->peekNext().type == TokenType::colon) {
        parseSomeVarDecStmt(lexer, parent);
        //consume ;
        lexer->lex();
    } else if(tok.type == TokenType::id && isAssignmentOp(lexer->peekNext().type)) {
        parseVarAssign(lexer, parent);
        //consume ;
        lexer->lex();
    } else {
        AssignNode* anode = new AssignNode();
        parseExpression(lexer, anode);
        if(isAssignmentOp(lexer->peek().type)) {
            anode->mtoken = lexer->peek();
            parent->addChild(anode);
            //consume =
            lexer->lex();
            parseExpression(lexer, anode);
        } else {
            parent->addChild(anode->mchildren.at(0));
            delete anode;
        }
        //consume ;
        lexer->lex();
    }
}

void parseIfblock(LexerTarget* lexer, AstNode* parent) {
/*
 * ifblock -> . if ( expression ) ifelsebody optelseblock
 */
    IfNode* ifnode = new IfNode();
    parent->addChild(ifnode);
    //consume if
    Token tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissLParen, tok, lexer);
    }
    //consume (
    lexer->lex();
    parseExpression(lexer, ifnode);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissRParen, tok, lexer);
    }
    //consume )
    lexer->lex();
    parseStatement(lexer, ifnode);
    /*
     * optelseblock -> . else ifelsebody | null
     */
    tok = lexer->peek();
    if(tok.type != TokenType::selse) {
        return;
    }
    //ElseNode* elsenode = new ElseNode();
    //parent->addChild(elsenode);
    //consume else
    lexer->lex();
    parseStatement(lexer, ifnode);

}

void parseLoop(LexerTarget* lexer, AstNode* parent) {
    /*
     * loop -> . loophead { stmtlistloop }
     * loophead -> for ( vardecassign ; conditional ; expr ) | while ( expression )
     */
    Token tok = lexer->peek();
    if(tok.type == TokenType::sfor) {
        parseForLoop(lexer, parent);
    } else if(tok.type == TokenType::swhile) {
        parseWhileLoop(lexer, parent);
    }

    //std::cout << "Parsing loop body\n";
    parseStatement(lexer, parent->lastChild());
}

void parseForLoop(LexerTarget* lexer, AstNode* parent) {
    //forloop -> . for ( vardecassign ; conditional ; expr )
    LoopNode* fornode = new LoopNode(AstNodeType::ForLoop);
    parent->addChild(fornode);
    //std::cout << "Parsing for loop!!\n";
    //consume for
    Token tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissLParen, tok, lexer);
    }
    //consume (
    lexer->lex();
    //std::cout << "Parsing for init\n"; 
    parseSomeVarDecStmt(lexer, fornode);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissSemicolon, tok, lexer);
    }
    //consume ;
    lexer->lex();
    //std::cout << "Parsing for conditional\n"; 
    parseExpression(lexer, fornode);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissSemicolon, tok, lexer);
    }
    //consume ;
    lexer->lex();
    //std::cout << "Parsing for update\n"; 
    parseExpression(lexer, fornode);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissRParen, tok, lexer);
    }
    //consume )
    lexer->lex();
}

void parseWhileLoop(LexerTarget* lexer, AstNode* parent) {
    //whileloop -> . while ( expression )
    LoopNode* whilenode = new LoopNode(AstNodeType::WhileLoop);
    parent->addChild(whilenode);
    //std::cout << "Parsing while loop!!\n";
    //consume while
    Token tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissLParen, tok, lexer);
    }
    //consume (
    lexer->lex();
    parseExpression(lexer, whilenode);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissRParen, tok, lexer);
    }
    //consume )
    lexer->lex();
}

void parseDeferBlock(LexerTarget* lexer, AstNode* parent) {
    //deferblock -> . defer stmt
    DeferStmtNode* defernode = new DeferStmtNode();
    parent->addChild(defernode);
    //consume defer
    lexer->lex();
    parseStatement(lexer, defernode);
}
void parseReturnStatement(LexerTarget* lexer, AstNode* parent) {
    /*
     * returnstmt -> . return expr ;
     * returnstmt -> . return ;
     */
    //consume return
    ReturnNode* retnode = new ReturnNode();
    parent->addChild(retnode);
    retnode->mtoken = lexer->peek();
    Token tok = lexer->lex();
    if(tok.type == TokenType::semicolon) {
        //consume ;
        lexer->lex();
        return;
    }
    parseExpression(lexer, retnode);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissSemicolon, tok, lexer);
    }
    //consume ;
    lexer->lex();
    return;
}

void parseExpression(LexerTarget* lexer, AstNode* parent) {
    auto ret = parseExpression(lexer,0);
    parent->addChild(ret);
    return;
}

void parseConst(LexerTarget* lexer, AstNode* parent) {
    // const -> ilit | flit | charlit | strlit
    //Consume token
    ConstantNode* constnode = new ConstantNode();
    constnode->setToken(lexer->peek());
    parent->addChild(constnode);
    auto op = lexer->peek().token;
    constnode->setVal(op);
    lexer->lex();
    return;
}

void parseScopedFunccall(LexerTarget* lexer, AstNode* parent) {
    //std::cout << "parsing some scoped function call!\n";
    auto scope = std::string(lexer->peek().token);
    //consume id ::
    lexer->lex();
    lexer->lex();
    auto tok = lexer->peekNext();
    if(tok.type ==TokenType::dblcolon) {
        parse_error(PET::MultipleScope,tok,lexer);
    }
    parseFunccall(lexer,parent);
    static_cast<FuncCallNode*>(parent->mchildren.back())->scopes = scope;
}

void parseFunccallOrVar(LexerTarget* lexer, AstNode* parent) {
    //get id
    Token tok = lexer->peek();
    //consume id, get potential (
    Token tokNext = lexer->peekNext();
    if(tokNext.type == TokenType::lparen) {
        //std::cout << "Parsing FuncCall!\n";
        parseFunccall(lexer, parent);
    } else if(tokNext.type == TokenType::dblcolon) {
        parseScopedFunccall(lexer, parent);
    } else {
        ////std::cout << "Parsing Variable!\n";
        //Will not work, have to fix somehow...
        //pass token into parseVar?
        //parseVar(lexer);    
        //for now just manually check if it is an id
        if(tok.type != TokenType::id) {
            //unneeded check? do we already know its an id?
            parse_error(PET::BadVarName, tok, lexer);
        }
        VarNode* varnode = new VarNode();
        varnode->addVarName(tok.token);
        varnode->mtoken = tok;
        if(parent != nullptr) {
            parent->addChild(varnode);
        }
        lexer->lex();
    }
}

void parseFunccall(LexerTarget* lexer, AstNode* parent) {
    //funccall -> . funcname ( opt_args )
    // funcname -> id
    Token tok = lexer->peek();
    FuncCallNode* funcallnode = new FuncCallNode();
    funcallnode->addFuncName(tok.token);
    funcallnode->mtoken = tok;
    if(parent != nullptr) {
        parent->addChild(funcallnode);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        //std::cout << __FUNCTION__ << ": token wasn't (, was " << tok.token << '\n';
        parse_error(PET::MissLParen, tok, lexer);
    }
    //consume (
    tok = lexer->lex();
    if(tok.type != TokenType::rparen) {
        parseOptargs(lexer, funcallnode);
    }
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        //std::cout << __FUNCTION__ << ": token wasn't ), was " << tok.token << '\n';
        parse_error(PET::MissRParen, tok, lexer);
    }
    //consume ')'
    lexer->lex();
    return;
}

void parseOptargs(LexerTarget* lexer, AstNode* parent) {
    //std::cout << "parsing optargs!\n";
    //opt_args -> null | id . | id . , opt_args2
    Token tok = lexer->peek();
    if(tok.type == TokenType::rparen) {
        return;
    }
    parseExpression(lexer, parent);
    tok = lexer->peek();
    if(tok.type == TokenType::comma) {
        //consume ,
        lexer->lex();
        //opt_args2 -> . id | . id , opt_args2
        while(1) {
            parseExpression(lexer, parent);
            Token tok = lexer->peek();
            if(tok.type != TokenType::comma) {
                break;
            }
            //consume ,
            lexer->lex();
        }

    }
    //Else return. if it is a ')' we will catch it in parseFunccall
    return;
}

void parseLoopStmt(LexerTarget* lexer, AstNode* parent) {
    /*
     * flowctrl -> break ; | continue ;
     */
    //std::cout << "Parsing a special loop statement\n";
    LoopStmtNode* brkcntnode = new LoopStmtNode();
    //consume break/continue
    if(lexer->peek().type == TokenType::sbreak) {
        brkcntnode->setBreak(true);
    } else {
        brkcntnode->setBreak(false);
    }
    brkcntnode->mtoken = lexer->peek();
    Token tok = lexer->lex();
    if(tok.type != TokenType::semicolon) {
        //consume ;
        //std::cout << "Token: " << tok.token << "\n";
        parse_error(PET::MissSemicolon, tok, lexer);
    }
    //std::cout << "parseLoopStmt finished, no problem\n";
    lexer->lex();
    parent->addChild(brkcntnode);
    return;
}

void parseStructDef(LexerTarget* lexer, AstNode* parent) {
    //structdef -> . struct id { element list }
    StructDefNode* structdef = new StructDefNode();
    
    //consume struct
    Token tok = lexer->lex();
    if(tok.type != TokenType::id) {
        //No identifier for struct definition
        parse_error(PET::Unknown, tok, lexer);
    }
    structdef->setToken(tok);
    structdef->ident = tok.token;
    
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::lbrace) {
        parse_error(PET::Unknown,tok, lexer);
    }
    //consume {
    lexer->lex();

    parseStructDefBody(lexer, structdef);
    tok = lexer->peek();
    if(tok.type != TokenType::rbrace) {
        //not a valid struct definition
        parse_error(PET::Unknown,tok, lexer);
    }
    //consume }
    lexer->lex();
    parent->addChild(structdef);
    return;
}

void parseStructDefBody(LexerTarget* lexer, AstNode* parent) {
    //elementlist -> vardec; elementlist
    //elementlist -> null
    
    while(lexer->peek().type != TokenType::rbrace) {
        //std::cout << "parsing member of struct\n";
        Token tokid = lexer->peek();
        //consume id
        lexer->lex();
        //consume :
        //Token tok = lexer->lex();
        lexer->lex();
        // it : . type
        
        //std::cout << "var " << tokid.token << " of type " << tok.token << "\n";
        
        //we have a declaration
        VarDeclNode* vdecnode = new VarDeclNode(AstNodeType::VarDec);
        VarNode* vnode = new VarNode();
        vnode->addVarName(tokid.token);
        vdecnode->addChild(vnode);
        parseType(lexer, vdecnode);
        vnode->mtypeinfo = vdecnode->mtypeinfo;
        //consume ;
        lexer->lex();
        parent->addChild(vdecnode);
    }
    return;
}


int getPrefixPrecedence(TokenType t) {
    switch(t) {
        case TokenType::ampersand:
        case TokenType::tilda:
        case TokenType::exclaim:
        case TokenType::dereference:
        case TokenType::plus:
        case TokenType::minus:
            return 11;
            break;
        default:
            break;
    }
    return 0;
}

int getInfixPrecedence(TokenType t) {
    switch(t) {
        case TokenType::cast:
            return 1;
            break;
        case TokenType::dblbar:
            return 2;
            break;
        case TokenType::dblampersand:
            return 3;
            break;
        case TokenType::bar:
            return 4;
            break;
        case TokenType::carrot:
            return 5;
            break;
        case TokenType::ampersand:
            return 6;
            break;
        case TokenType::equality:
        case TokenType::nequality:
            return 7;
            break;
        case TokenType::lessthan:
        case TokenType::ltequal:
        case TokenType::greaterthan:
        case TokenType::gtequal:
            return 8;
            break;
        case TokenType::plus:
        case TokenType::minus:
            return 9;
            break;
        case TokenType::mod:
        case TokenType::star:
        case TokenType::fslash:
            return 10;
            break;
        case TokenType::dot:
        case TokenType::lsqrbrace:
            return 12;
            break;
        default:
            break;
    }
    return 0;
}

int getNextPrecedence(LexerTarget* lexer) {
    return getInfixPrecedence(lexer->peek().type);
}

int isRightAssociative(TokenType t) {
    return 0;
}

AstNode* parsePrefix(LexerTarget* lexer) {
    Token tok = lexer->peek();
    //consume prefix/unary op token
    lexer->lex();
    int precedence = getPrefixPrecedence(tok.type);
    AstNode* child = parseExpression(lexer, precedence);
    auto opnode = new BinOpNode();
    opnode->setToken(tok);
    opnode->unaryOp = true;
    opnode->addChild(child);
    return opnode;
}

AstNode* parseInfix(LexerTarget* lexer) {
    Token tok = lexer->peek();
    //consume infix/binop token
    lexer->lex();

    //get precedence
    int precedence = getInfixPrecedence(tok.type);
    precedence -= isRightAssociative(tok.type);

    AstNode* child = parseExpression(lexer, precedence);

    //make node
    auto opnode = new BinOpNode();
    opnode->setToken(tok);
    opnode->addChild(child);
    opnode->addChild(child); //when this is returned, the left child will be set
    return opnode;
}

AstNode* parseParenGroup(LexerTarget* lexer) {
    //consume (
    lexer->lex();
    auto child = parseExpression(lexer, 0);
    Token t = lexer->peek();
    if(t.type != TokenType::rparen) {
        parse_error(PET::MissRParen, t, lexer);
    }
    //consume )
    lexer->lex();
    return child;
}

AstNode* parseArrInd(LexerTarget* lexer) {
    Token tok = lexer->peek();
    //consume [ token
    lexer->lex();

    AstNode* child = parseExpression(lexer, 0);

    if(lexer->peek().type != TokenType::rsqrbrace) {
        std::cout << "Error: expected '[' token!\n";
        exit(0);
    }

    //consume ] token
    lexer->lex();

    //make node
    auto opnode = new BinOpNode();
    opnode->setToken(tok);
    opnode->addChild(child);
    opnode->addChild(child); //when this is returned, the left child will be set
    return opnode;
}

AstNode* parseCast(LexerTarget* lexer, AstNode* child) {
    //consume cast
    lexer->lex();
    CastNode* cnode = new CastNode();
    cnode->mchildren.reserve(1);
    parseType(lexer, cnode);
    cnode->toType = cnode->mtypeinfo;
    cnode->addChild(child);
    return cnode;
}

AstNode* parseExpression(LexerTarget* lexer, int precedence) {
/*
 * cast
 * ||
 * &&
 * |
 * ^
 * == !=
 * < > <= >=
 * + -
 * / *
 * & @ ! ~ +
 * [] . (call)
 * ( )
 */
    Token t = lexer->peek();
    AstNode* left = nullptr;
    //determine function/parslette/prefix
    switch(t.type) {
        case TokenType::intlit:
        case TokenType::strlit:
        case TokenType::charlit:
        case TokenType::floatlit:
            {
                AstNode temp;
                temp.mchildren.reserve(2);
                parseConst(lexer, &temp);
                left = temp.mchildren.at(0);
            }
            break;
        case TokenType::id:
            {
                AstNode temp;
                temp.mchildren.reserve(1);
                parseFunccallOrVar(lexer, &temp);
                left = temp.mchildren.at(0);
            }
            break;
        case TokenType::ampersand:
        case TokenType::tilda:
        case TokenType::exclaim:
        case TokenType::dereference:
        case TokenType::plus:
        case TokenType::minus:
            {
                left = parsePrefix(lexer);
            }
            break;
        case TokenType::lparen:
            {
                left = parseParenGroup(lexer);
            }
            break;
        case TokenType::ssizeof:
        {
            //consume sizeof
            lexer->lex();
            if(lexer->peek().type != TokenType::lparen) {
                parse_error(PET::MissLParen, t, lexer);
            }
            //consume (
            lexer->lex();
            //parseType
            auto sizeofn = new SizeOfNode();
            parseType(lexer,sizeofn);
            if(lexer->peek().type != TokenType::rparen) {
                parse_error(PET::MissRParen,t,lexer);
            }
            //consume )
            lexer->lex();
            left = sizeofn;
        }
            break;
        default:
            //error
            std::cout << "Error: expected '[' token!\n";
            exit(0);
            break;
    }

    while(precedence < getNextPrecedence(lexer)) {
        t = lexer->peek();
        //determine which infix op to do
        switch(t.type) {
            case TokenType::dot:
            case TokenType::bar:
            case TokenType::dblbar:
            case TokenType::ampersand:
            case TokenType::dblampersand:
            case TokenType::mod:
            case TokenType::plus:
            case TokenType::minus:
            case TokenType::star:
            case TokenType::fslash:
            case TokenType::carrot:
            case TokenType::equality:
            case TokenType::nequality:
            case TokenType::lessthan:
            case TokenType::ltequal:
            case TokenType::greaterthan:
            case TokenType::gtequal:
                {
                    BinOpNode* tmpN = static_cast<BinOpNode*>(parseInfix(lexer));
                    tmpN->setLHS(left);
                    left = tmpN;
                }
                break;
            case TokenType::lsqrbrace:
                {
                    BinOpNode* tmpN = static_cast<BinOpNode*>(parseArrInd(lexer));
                    tmpN->setLHS(left);
                    left = tmpN;
                }
                break;
            case TokenType::cast:
                {
                    auto tmpN = parseCast(lexer, left);
                    left = tmpN;
                }
            break;
            default:
                //error
                exit(0);
                break;
        }
    }
    return left;
}
