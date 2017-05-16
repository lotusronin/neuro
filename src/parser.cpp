#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <assert.h>
#include "parser.h"
#include "tokens.h"
#include "astnodetypes.h"

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
void parseOptparams2(LexerTarget* lexer, AstNode* parent);
void parseType(LexerTarget* lexer, AstNode* parent);
void parseVar(LexerTarget* lexer, AstNode* parent);
void parseVarDec(LexerTarget* lexer, AstNode* parent);
void parseVarDecAssign(LexerTarget* lexer, AstNode* parent);
void parseVarAssign(LexerTarget* lexer, AstNode* parent);
void parseSomeVarDecStmt(LexerTarget* lexer, AstNode* parent);
void parseFunctionDef(LexerTarget* lexer, AstNode* parent);
void parseBlock(LexerTarget* lexer, AstNode* parent);
void parseStatementList(LexerTarget* lexer, AstNode* parent);
void parseStatement(LexerTarget* lexer, AstNode* parent);
void parseIfblock(LexerTarget* lexer, AstNode* parent);
void parseOptElseBlock(LexerTarget* lexer, AstNode* parent);
void parseLoop(LexerTarget* lexer, AstNode* parent);
void parseForLoop(LexerTarget* lexer, AstNode* parent);
void parseDeferBlock(LexerTarget* lexer, AstNode* parent);
void parseWhileLoop(LexerTarget* lexer, AstNode* parent);
void parseReturnStatement(LexerTarget* lexer, AstNode* parent);
void parseExpression(LexerTarget* lexer, AstNode* parent);
AstNode* parseExpression(LexerTarget* lexer);
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
void parseOptargs2(LexerTarget* lexer, AstNode* parent);
void parseLoopStmt(LexerTarget* lexer, AstNode* parent);
void parsePrototypeOrStruct(LexerTarget* lexer, CompileUnitNode* parent);

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
    std::cout << "astnode " << sizeof(AstNode) << '\n';
    std::cout << "program " << sizeof(ProgramNode) << '\n';
    std::cout << "compileunit " << sizeof(CompileUnitNode) << '\n';
    std::cout << "prototype " << sizeof(PrototypeNode) << '\n';
    std::cout << "funcdef " << sizeof(FuncDefNode) << '\n';
    std::cout << "params " << sizeof(ParamsNode) << '\n';
    std::cout << "block " << sizeof(BlockNode) << '\n';
    std::cout << "vdeca " << sizeof(VarDecAssignNode) << '\n';
    std::cout << "vdec " << sizeof(VarDecNode) << '\n';
    std::cout << "for " << sizeof(ForLoopNode) << '\n';
    std::cout << "while " << sizeof(WhileLoopNode) << '\n';
    std::cout << "return " << sizeof(ReturnNode) << '\n';
    std::cout << "defer " << sizeof(DeferStmtNode) << '\n';
    std::cout << "loopstmt " << sizeof(LoopStmtNode) << '\n';
    std::cout << "if " << sizeof(IfNode) << '\n';
    std::cout << "funccall " << sizeof(FuncCallNode) << '\n';
    std::cout << "var " << sizeof(VarNode) << '\n';
    std::cout << "binop " << sizeof(BinOpNode) << '\n';
    std::cout << "constant " << sizeof(ConstantNode) << '\n';
    std::cout << "assign " << sizeof(AssignNode) << '\n';
    std::cout << "structdef " << sizeof(StructDefNode) << '\n';
    std::cout << "cast " << sizeof(CastNode) << '\n';
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
        parsePrototypeOrStruct(lexer, (CompileUnitNode*)parent);
        //parsePrototype(lexer, (CompileUnitNode*)parent);
        ////std::cout << "prototype matched\n";
    } else if(tok.type == TokenType::tstruct) {
        parseStructDef(lexer, parent);
    } else if(tok.type == TokenType::eof) {
        //std::cout << "File is parsed, no errors detected!\n\n";
        return;
    }else {
        parse_error(ParseErrorType::BadTopLevelStatement, tok, lexer);
    }
    parseTopLevelStatements(lexer, parent);
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
        ((CompileUnitNode*)parent)->imports.push_back(newfilename);
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
        ((StructDefNode*)(parent->mchildren.back()))->foreign = true;
        std::cout << "opaque struct found!\n";
    } else {
        parse_error(PET::MissForeign, tok, lexer);
    }
}

void parsePrototype(LexerTarget* lexer, CompileUnitNode* parent) {
    //prototypes -> . extern fn id ( opt_params ) : type ;
    PrototypeNode* protonode = new PrototypeNode();
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
        parseOptparams2(lexer, parent);
    }
}

void parseOptparams2(LexerTarget* lexer, AstNode* parent) {
    //opt_params2 -> id : type | id : type , opt_params2
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
    if(tok.type == TokenType::comma) {
        //consume ,
        lexer->lex();
        parseOptparams2(lexer, parent);
    }
    return;
}

static bool isTokenAType(TokenType t) {
    //Get rid of the if-else-if flow in parseType
    //TODO(marcus): may want to combine this with tokenTypeIsAType()
    switch(t) {
        case TokenType::tint:
        case TokenType::tuint:
        case TokenType::tchar:
        case TokenType::tuchar:
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
    //tnode->mindirection = indirection;
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
        default:
            mstype = SemanticType::Typeless;
            break;
    }
    /**/
    //tnode->setToken(tok);
    //t.type = tnode->mstype;
    t.type = mstype;
    parent->mtypeinfo = t;
    //parent->mstype = t.type;
    if(isTokenAType(tok.type)) {
        //consume int/char/bool/float/double/void/id
        lexer->lex();
    } else {
        parse_error(PET::BadTypeIdentifier, tok, lexer);
    }
    
    //delete tnode;
    /*
    if(parent) {
        //null check
        parent->addChild(tnode);
    } else {
        delete tnode;
    }*/
    return;
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

//TODO(marcus): Uncalled, should we use it
//or just get rid of it?
void parseVarDec(LexerTarget* lexer, AstNode* parent) {
    //vardec -> var : type
    assert(false);
    parseVar(lexer, nullptr);
    Token tok = lexer->peek();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissColon, tok, lexer);
    }
    //consume :
    lexer->lex();
    parseType(lexer, nullptr);
    return;
}

//TODO(marcus): Uncalled, should we use it
//or just get rid of it?
void parseVarDecAssign(LexerTarget* lexer, AstNode* parent) {
    //vardecassign -> vardec = expression
    assert(false);
    parseVarDec(lexer, nullptr);
    Token tok = lexer->peek();
    if(tok.type != TokenType::assignment) {
        parse_error(PET::MissEqVarDecAssign, tok, lexer);
    }
    //consume =
    lexer->lex();
    parseExpression(lexer, nullptr);
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
    if(tok.type != TokenType::assignment) {
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

void parseSomeVarDecStmt(LexerTarget* lexer, AstNode* parent) {
    //std::cout << "parsing some sort of var declaration.\n";
    Token tokid = lexer->peek();
    //consume id
    lexer->lex();
    //consume :
    Token tok = lexer->lex();
    //Token nextTok = lexer->peekNext();
    // id : . type = expression
    // it : . type
    if(tok.type == TokenType::assignment) {
        //std::cout << "var declaration is assignment, type inferred.\n";
        //std::cout << "current: " << tok.token << " next: " << nextTok.token << "\n";
        // id : . = expression
        //we have type inferenced declaration assignment
        VarDecAssignNode* vdecassignnode = new VarDecAssignNode();
        vdecassignnode->mtoken = tok;
        VarNode* vnode = new VarNode();
        vnode->mtoken = tokid;
        vnode->addVarName(tokid.token);
        vdecassignnode->addChild(vnode);
        parent->addChild(vdecassignnode);
        //consume =
        lexer->lex();
        parseExpression(lexer, vdecassignnode);
    } else if(tokenTypeIsAType(tok.type)) {
        VarNode* vnode = new VarNode();
        vnode->addVarName(tokid.token);
        vnode->mtoken = tokid;
        //TODO(marcus): parse type, then find if you're an assignment or declaration
        parseType(lexer, vnode);
        if(lexer->peek().type == TokenType::assignment) {
            //std::cout << "var declaration is assignment, type given.\n";
            //std::cout << "current: " << lexer->peek().token << " next: " << lexer->peekNext().token << "\n";
            //we have a declaration and assignment
            VarDecAssignNode* vdecassignnode = new VarDecAssignNode();
            vdecassignnode->mtoken = lexer->peek();
            vdecassignnode->addChild(vnode);
            parent->addChild(vdecassignnode);
            //consume =
            lexer->lex();
            //std::cout << "after parseType call and =\n";
            //std::cout << "current: " << lexer->peek().token << " next: " << lexer->peekNext().token << "\n";
            parseExpression(lexer, vdecassignnode);
        } else {
            //std::cout << "var declaration only\n";
            //std::cout << "current: " << tok.token << " next: " << nextTok.token << "\n";
            //we have a declaration
            VarDecNode* vdecnode = new VarDecNode();
            vdecnode->addChild(vnode);
            parent->addChild(vdecnode);
        }
    } else {
        parse_error(PET::MissColon, tok, lexer);
    }
}

void parseFunctionDef(LexerTarget* lexer, AstNode* parent) {
    //functiondefs -> . fn id ( opt_params ) : type block
    //consume fn
    FuncDefNode* funcnode = new FuncDefNode();
    parent->addChild(funcnode);
    Token tok = lexer->lex();
    //std::cout << tok.token << '\n';
    if(tok.type != TokenType::id) {
        parse_error(PET::BadPrototypeName, tok, lexer);
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
    parseStatement(lexer, parent);
    //std::cout << "Statement matched\n";
    Token tok = lexer->peek();
    if(tok.type != TokenType::rbrace) {
        parseStatementList(lexer, parent);
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
    } else if(tok.type == TokenType::id && lexer->peekNext().type == TokenType::assignment) {
        parseVarAssign(lexer, parent);
        //consume ;
        lexer->lex();
    } else {
        AssignNode* anode = new AssignNode();
        parseExpression(lexer, anode);
        if(lexer->peek().type == TokenType::assignment) {
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
    parseOptElseBlock(lexer, ifnode);
}

void parseOptElseBlock(LexerTarget* lexer, AstNode* parent) {
    /*
     * optelseblock -> . else ifelsebody | null
     */
    Token tok = lexer->peek();
    if(tok.type != TokenType::selse) {
        return;
    }
    //ElseNode* elsenode = new ElseNode();
    //parent->addChild(elsenode);
    //consume else
    lexer->lex();
    parseStatement(lexer, parent);
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
    ForLoopNode* fornode = new ForLoopNode();
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
    WhileLoopNode* whilenode = new WhileLoopNode();
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
    auto ret = parseExpression(lexer);
    parent->addChild(ret);
    return;
}

AstNode* parseExpression(LexerTarget* lexer) {
    /*
     * expr -> orexpr cast type | orexpr
     */
    auto child = parseLogicalOr(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::cast) {
        //consume cast
        lexer->lex();
        CastNode* cnode = new CastNode();
        parseType(lexer, cnode);
        cnode->toType = cnode->mtypeinfo;
        cnode->addChild(child);
        return cnode;
    }
    return child;
}

AstNode* parseLogicalOr(LexerTarget* lexer) {
/* 
 * expr -> exprlogicaland dblbar expr  | exprlogicaland
 */
    ////std::cout << "Parsing Expression!\n";
    auto child = parseLogicalAnd(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::dblbar) {
        BinOpNode* opnode = new BinOpNode();
        auto s = std::string("expression");
        opnode->setToken(tok);
        opnode->setOp(s);
        //consume ||
        lexer->lex();
        auto child2 = parseExpression(lexer);
        opnode->addChild(child);
        opnode->addChild(child2);
        return opnode;
    }
    return child;
}

AstNode* parseLogicalAnd(LexerTarget* lexer) {
/* 
 * exprand -> exprbitor dblampersand exprlogicaland  | exprbitor
 */
    ////std::cout << "Parsing Logical And Expression!\n";
    auto ret = parseBitOr(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::dblampersand) {
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        //consume &&
        lexer->lex();
        auto ret2 = parseLogicalAnd(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    }
    return ret;
}

AstNode* parseBitOr(LexerTarget* lexer) {
/* 
 * exprbitor -> exprxor bar exprbitor  | exprxor
 */
    ////std::cout << "Parsing Bitwise Or Expression!\n";
    auto ret = parseBitXor(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::bar) {
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        //consume |
        lexer->lex();
        auto ret2 = parseBitOr(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    }
    return ret;
}

AstNode* parseBitXor(LexerTarget* lexer) {
/* 
 * exprxor -> expreqneq carrot exprxor  | expreqneq
 */
    ////std::cout << "Parsing Bitwise Xor Expression!\n";
    auto ret = parseEqneq(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::carrot) {
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        //consume ^
        lexer->lex();
        auto ret2 = parseBitXor(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    }
    return ret;
}

AstNode* parseEqneq(LexerTarget* lexer) {
/* 
 * expreqneq -> exprglte eqneq expreqneq  | exprglte
 */
    ////std::cout << "Parsing EqNeq Expression!\n";
    auto ret = parseGLTE(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::equality || tok.type == TokenType::nequality) {
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        //consume == or !=
        lexer->lex();
        auto ret2 = parseEqneq(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    }
    return ret;
}

AstNode* parseGLTE(LexerTarget* lexer) {
/* 
 * exprgtle -> exprplusmin gtelte exprgtle  | exprplusmin
 */
    ////std::cout << "Parsing GTELTE Expression!\n";
    auto ret = parsePlusmin(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::ltequal || tok.type == TokenType::gtequal || tok.type == TokenType::greaterthan || tok.type == TokenType::lessthan) {
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        //consume < or > or <= or >=
        lexer->lex();
        auto ret2 = parseGLTE(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    }
    return ret;
}

AstNode* parsePlusmin(LexerTarget* lexer) {
/* 
 * exprplusmin -> multdiv plusmin exprplusmin  | multdiv
 */
    ////std::cout << "Parsing PlusMin Expression!\n";
    auto ret = parseMultdiv(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::plus || tok.type == TokenType::minus) {
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        //consume + or -
        lexer->lex();
        auto ret2 = parsePlusmin(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    }
    return ret;
}

AstNode* parseMultdiv(LexerTarget* lexer) {
 /* 
  * multdiv -> addrofindir starslash multdiv | addrofindir
  */
    ////std::cout << "Parsing MultDiv Expression!\n";
    auto ret = parseAddrOfIndir(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::star || tok.type == TokenType::fslash || tok.type == TokenType::mod) {
        //consume token
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        lexer->lex();
        auto ret2 = parseMultdiv(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    }
    return ret;
}

AstNode* parseAddrOfIndir(LexerTarget* lexer) {
 /* 
  * addrof-indir -> &memberaccess | *memberaccess | !memberaccess | ~memberaccess | memberaccess
  */    
    ////std::cout << "Parsing AddrOfIndir Expression!\n";
    BinOpNode* opnode = nullptr;
    Token tok = lexer->peek();
    if(tok.type == TokenType::dereference || tok.type == TokenType::ampersand || tok.type == TokenType::exclaim || tok.type == TokenType::tilda || tok.type == TokenType::minus || tok.type == TokenType::plus) {
        //consume token (@ or & or ! or ~ or - or +)
        opnode = new BinOpNode();
        opnode->setToken(tok);
        opnode->unaryOp = true;
        opnode->mpriority = 5; //TODO(marcus): fix this with a better way for telling binop nodes they are unary
        lexer->lex();
    }
     auto ret = parseMemberAccess(lexer);
     if(opnode) {
         opnode->addChild(ret);
         return opnode;
     }
     return ret;
}

AstNode* parseMemberAccess(LexerTarget* lexer) {
    /*
     * memberaccess -> parenexp dotarrow memberaccess | parenexp
     */
    ////std::cout << "Parsing MemberAccess Expression!\n";
    auto ret = parseParenexp(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::rsqrbrace) {
        tok = lexer->lex();
    }
    if(tok.type == TokenType::dot) {
        //consume token
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        lexer->lex();
        auto ret2 = parseMemberAccess(lexer);
        opnode->addChild(ret);
        opnode->addChild(ret2);
        return opnode;
    } else if(tok.type == TokenType::lsqrbrace) {
        //std::cout << "Parsing Array access!\n";
        auto opnode = new BinOpNode();
        opnode->setToken(tok);
        opnode->addChild(ret);
        //consume [
        lexer->lex();
        //parseExpression(lexer, opnode);
        auto ret2 = parseMemberAccess(lexer);
        opnode->addChild(ret2);
        //std::cout << "Expression parsed\n";
        //std::cout << "Next token is " << lexer->peek().token << '\n';
        //lexer->lex();
        return opnode;
    }
    return ret;
}

bool tokenTypeIsLiteral(TokenType t) {
    switch(t) {
        case TokenType::intlit:
        case TokenType::floatlit:
        case TokenType::strlit:
        case TokenType::charlit:
            return true;
            break;
        default:
            break;
    }
    return false;
}

AstNode* parseParenexp(LexerTarget* lexer) {
 /* 
  * parenexp -> ( expr ) | const | var | funcall
  */
    BinOpNode parent;
    BinOpNode::constructed--;
    BinOpNode::deleted--;
    ////std::cout << "Parsing Paren Expression!\n";
    auto s = std::string("parenexpr");
    Token tok = lexer->peek();
    if(tok.type == TokenType::lparen) {
        BinOpNode* opnode = new BinOpNode();
        opnode->setToken(tok);
        s = std::string("( )");
        opnode->setOp(s);
        //consume (
        lexer->lex();
        parseExpression(lexer, opnode);
        tok = lexer->peek();
        if(tok.type != TokenType::rparen) {
            parse_error(PET::MissRParen, tok, lexer);
        }
        //consume )
        lexer->lex();
        return opnode;
    } else if(tokenTypeIsLiteral(tok.type)) {
        parseConst(lexer, &parent);
    } else if(tok.type == TokenType::id) {
        parseFunccallOrVar(lexer, &parent);
    } else if(tok.type == TokenType::ssizeof) {
        //consume sizeof
        lexer->lex();
        //std::cout << "parsing sizeof!\n";
        //std::cout << lexer->peek().token << '\n';
        if(lexer->peek().type != TokenType::lparen) {
            parse_error(PET::MissLParen, tok, lexer);
        }
        //consume (
        lexer->lex();
        //parseType
        auto sizeofn = new SizeOfNode();
        //std::cout << "parsing sizeof Type!\n";
        parseType(lexer,sizeofn);
        if(lexer->peek().type != TokenType::rparen) {
            parse_error(PET::MissRParen,tok, lexer);
        }
        //consume )
        lexer->lex();
        //std::cout << "parsing sizeof done!\n";
        //std::cout << "Next is " << lexer->peek().token << '\n';
        return sizeofn;
    } else {
        //assert(false);
        parse_error(PET::Unknown, tok, lexer);
    }
    return parent.mchildren[0];
}

void parseConst(LexerTarget* lexer, AstNode* parent) {
    // const -> ilit | flit | charlit | strlit
    //Consume token
    ConstantNode* constnode = new ConstantNode();
    constnode->setToken(lexer->peek());
    parent->addChild(constnode);
    std::string op = lexer->peek().token;
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
    ((FuncCallNode*)(parent->mchildren.back()))->scopes = scope;
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
        parseOptargs2(lexer, parent);
    }
    //Else return. if it is a ')' we will catch it in parseFunccall
    return;
}

void parseOptargs2(LexerTarget* lexer, AstNode* parent) {
    //opt_args2 -> . id | . id , opt_args2
    Token tok = lexer->peek();
    parseExpression(lexer, parent);
    tok = lexer->peek();
    if(tok.type == TokenType::comma) {
        //consume ,
        lexer->lex();
        parseOptargs2(lexer, parent);
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
        VarDecNode* vdecnode = new VarDecNode();
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
