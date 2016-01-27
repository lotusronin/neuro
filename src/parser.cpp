#include <iostream>
#include <vector>
#include "parser.h"
#include "tokens.h"

//ParseErrorType:: is a pain to write out every time... :)
#define PET ParseErrorType

void parseTopLevelStatements(LexerTarget* lexer);
void parseImportStatement(LexerTarget* lexer);
void parsePrototype(LexerTarget* lexer);
void parseOptparams(LexerTarget* lexer);
void parseOptparamsTail(LexerTarget* lexer);
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
void parseFunccallOrVar(LexerTarget* lexer);
void parseFunccall(LexerTarget* lexer);
void parseOptargs(LexerTarget* lexer);
void parseOptargs2(LexerTarget* lexer);

int parse_error(ParseErrorType type, Token& t) {
    switch (type) {
        case ParseErrorType::BadTopLevelStatement:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad Top Level Statement\n";
            std::cout << "  Token: " << t.token << " Is not a valid start of a top level statement\n";
            break;
        case ParseErrorType::BadImportName:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Invalid import value\n";
            std::cout << "  Token: " << t.token << " Cannot be imported\n";
            break;
        case ParseErrorType::MissImportSemicolon:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing semicolon\n";
            std::cout << "  Token: " << t.token << " is not a ';'!\n";
            break;
        case ParseErrorType::MissPrototypeFn:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expecting fn keyword\n";
            std::cout << "  Token: " << t.token << " is not 'fn'!\n";
            break;
        case ParseErrorType::BadPrototypeName:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad prototype identifier\n";
            std::cout << "  Token: " << t.token << " is not a valid prototype name!\n";
            break;
        case ParseErrorType::MissPrototypeLParen:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Prototype missing left parentheses\n";
            std::cout << "  Token: " << t.token << " is not a '('!\n";
            break;
        case ParseErrorType::BadFunctionParameter:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad function parameter\n";
            std::cout << "  Token: " << t.token << " Expected an identifier or ')'!\n";
            break;
        case ParseErrorType::MissOptparamColon:
            std::cout << "File:" << t.line << ":" << t.col << " Error: parameter missing colon\n";
            std::cout << "  Token: " << t.token << " is not a ':'!\n";
            break;
        case ParseErrorType::BadTypeIdentifier:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad type identifier\n";
            std::cout << "  Token: " << t.token << " is not a type!\n";
            break;
        case ParseErrorType::BadOptparamTail:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad parameter formation\n";
            std::cout << "  Token: " << t.token << " Expected a ',' or a ')'!\n";
            break;
        case ParseErrorType::MissPrototypeColon:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expecting colon in prototype\n";
            std::cout << "  Token: " << t.token << " is not a ':'!\n";
            break;
        case ParseErrorType::MissPrototypeSemicolon:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing semicolon\n";
            std::cout << "  Token: " << t.token << " is not a ';'!\n";
            break;
        case ParseErrorType::BadVarName:
            std::cout << "File:" << t.line << ":" << t.col << " Error: bad variable name\n";
            std::cout << "  Token: " << t.token << " is not a valid variable identifier!\n";
            break;
        case ParseErrorType::MissVardecColon:
            std::cout << "File:" << t.line << ":" << t.col << " Error: missing colon in variable declaration\n";
            std::cout << "  Token: " << t.token << " is not a ':'!\n";
            break;
        case ParseErrorType::MissPrototypeRParen:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Prototype missing right parentheses\n";
            std::cout << "  Token: " << t.token << " is not a ')'!\n";
            break;
        case ParseErrorType::IncompleteBlock:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Block was never closed\n";
            std::cout << "  Token: " << t.token << " is not a '}'!\n";
            break;
        case ParseErrorType::MissIfLParen:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing opening parentheses in if statement\n";
            std::cout << "  Token: " << t.token << " is not a '('!\n";
            break;
        case ParseErrorType::MissIfRParen:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing closing parentheses in if statement\n";
            std::cout << "  Token: " << t.token << " is not a ')'!\n";
            break;
        case ParseErrorType::MissLParenFor:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing opening parentheses in for loop\n";
            std::cout << "  Token: " << t.token << " is not a '('!\n";
            break;
        case ParseErrorType::MissSemicolonFor1:
            std::cout << "File:" << t.line << ":" << t.col << " Error: missing semicolon in for loop before condition\n";
            std::cout << "  Token: " << t.token << " is not a ';'!\n";
            break;
        case ParseErrorType::MissSemicolonFor2:
            std::cout << "File:" << t.line << ":" << t.col << " Error: missing semicolon in for loop after condition\n";
            std::cout << "  Token: " << t.token << " is not a ';'!\n";
            break;
        case ParseErrorType::MissRParenFor:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing closing parentheses in for loop\n";
            std::cout << "  Token: " << t.token << " is not a ')'!\n";
            break;
        case ParseErrorType::MissLParenWhile:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing opening parentheses in while loop\n";
            std::cout << "  Token: " << t.token << " is not a '('!\n";
            break;
        case ParseErrorType::MissRParenWhile:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Missing closing parentheses in while loop\n";
            std::cout << "  Token: " << t.token << " is not a ')'!\n";
            break;
        case ParseErrorType::MissEqVarDecAssign:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Malformed variable declaration assignment\n";
            std::cout << "  Token: " << t.token << " is not a '='!\n";
            break;
       default:
            std::cout << "Unknown Parse Error!\n";
            break;
    }
    exit(-1);
    return 0;
}

Parser::Parser(LexerTarget* _lexer) {
    mlexer = _lexer;
}

Parser::~Parser() {
}

void Parser::setLexer(LexerTarget* _lexer) {
    mlexer = _lexer;
}

void Parser::parse() {
    //TODO(marcus) make Translation unit/program parse node
    std::cout << "Beginning parse!\n";
    mlexer->lex();
    parseTopLevelStatements(mlexer);
}

void parseTopLevelStatements(LexerTarget* lexer) {
    //tl_statements -> imports tl_statements 
    //tl_statements -> prototypes tl_statements
    //tl_statements -> functiondefs tl_statements 
    //tl_statements -> null
    Token tok = lexer->peek();
    if(tok.type == TokenType::import) {
        std::cout << "import token, beginning to match import statement...\n";
        parseImportStatement(lexer);
        std::cout << "import statement matched\n";
    } else if(tok.type == TokenType::fn) {
        std::cout << "Function Definitions Parse\n";
        parseFunctionDef(lexer);
        std::cout << "Function Definitions matched\n";
    } else if(tok.type == TokenType::foreign) {
        std::cout << "extern token, beginning to match prototype...\n";
        parsePrototype(lexer);
        std::cout << "prototype matched\n";
    } else if(tok.type == TokenType::eof) {
        std::cout << "File is parsed, no errors detected!\n\n";
        return;
    }else {
        parse_error(ParseErrorType::BadTopLevelStatement, tok);
    }
    parseTopLevelStatements(lexer);
}

void parseImportStatement(LexerTarget* lexer) {
    //imports -> . import id ;
    //consume import
    Token tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadImportName, tok);
    }
    std::string newfilename = tok.token+".nro";
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::semicolon) {
        parse_error(ParseErrorType::MissImportSemicolon, tok);
    }
    //consume ;
    lexer->lex();
    LexerTarget importlex = LexerTarget(newfilename, lexer->isDebug());
    importlex.lex();
    std::cout << "\nImporting file: " << newfilename << "\n";
    parseTopLevelStatements(&importlex);
    return;
}

void parsePrototype(LexerTarget* lexer) {
    //prototypes -> . extern fn id ( opt_params ) : type ;
    //consume extern
    Token tok = lexer->lex();
    if(tok.type != TokenType::fn) {
        parse_error(PET::MissPrototypeFn, tok);
    }
    //consume fn
    tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadPrototypeName, tok);
    }
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(ParseErrorType::MissPrototypeLParen, tok);
    }
    //consume (
    lexer->lex();
    parseOptparams(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissPrototypeRParen, tok);
    }
    //consume )
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissPrototypeColon, tok);
    }
    //consume :
    lexer->lex();
    parseType(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissPrototypeSemicolon, tok);
    }
    //consume ;
    tok = lexer->lex();
}

void parseOptparams(LexerTarget* lexer) {
    //opt_params -> null | id : type opt_params_tail
    Token tok = lexer->peek();
    if(tok.type == TokenType::rparen) {
        return;
    }
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadFunctionParameter, tok);
    }
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissOptparamColon, tok);
    }
    //consume :
    lexer->lex();
    parseType(lexer);
    parseOptparamsTail(lexer);
}

void parseOptparamsTail(LexerTarget* lexer) {
    //opt_params_tail -> null | , opt_params
    //TODO(marcus): currently allows prototypes like func(a:int,)
    //should not allow this, rewrite like optargs
    Token tok = lexer->peek();
    if(tok.type == TokenType::comma) {
        //consume ,
        lexer->lex();
        parseOptparams(lexer);
    } else if(tok.type == TokenType::rparen) {
        return;
    } else {
        parse_error(PET::BadOptparamTail, tok);
    }
}

void parseType(LexerTarget* lexer) {
    //type -> int | char | float | double | bool | id
    Token tok = lexer->peek();
    if(tok.type == TokenType::tint) {
        //consume int
        lexer->lex();
        return;
    } else if(tok.type == TokenType::tchar) {
        //consume char
        lexer->lex();
        return;
    } else if(tok.type == TokenType::tbool) {
        //consume bool
        lexer->lex();
        return;
    } else if(tok.type == TokenType::tfloat) {
        //consume float
        lexer->lex();
        return;
    } else if(tok.type == TokenType::tdouble) {
        //consume double
        lexer->lex();
        return;
    } else if(tok.type == TokenType::id) {
        //consume id
        lexer->lex();
        return;
    } else {
        parse_error(PET::BadTypeIdentifier, tok);
    }
}

/*
 *varstmt -> vardec | vardecassign | varassign
 *varassign -> var = expression
 *vardecassign -> vardec = expression
 */
void parseVar(LexerTarget* lexer) {
    //var -> id
    Token tok = lexer->peek();
    if(tok.type != TokenType::id) {
        parse_error(PET::BadVarName, tok);
    }
    //consume id
    lexer->lex();
    return;
}

void parseVarDec(LexerTarget* lexer) {
    //vardec -> var : type
    parseVar(lexer);
    Token tok = lexer->peek();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissVardecColon, tok);
    }
    //consume :
    lexer->lex();
    parseType(lexer);
    return;
}

void parseVarDecAssign(LexerTarget* lexer) {
    //vardecassign -> vardec = expression
    parseVarDec(lexer);
    Token tok = lexer->peek();
    if(tok.type != TokenType::assignment) {
        parse_error(PET::MissEqVarDecAssign, tok);
    }
    //consume =
    lexer->lex();
    parseExpression(lexer);
    return;
}

void parseFunctionDef(LexerTarget* lexer) {
    //functiondefs -> . fn id ( opt_params ) : type block
    //consume fn
    Token tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(PET::BadPrototypeName, tok);
    }
    //consume id
    tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissPrototypeLParen,tok);
    }
    //consume (
    lexer->lex();
    parseOptparams(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissPrototypeRParen,tok);
    }
    //consume )
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissPrototypeColon,tok);
    }
    //consume :
    lexer->lex();
    parseType(lexer);
    parseBlock(lexer);
}

void parseBlock(LexerTarget* lexer) {
    //block -> . { stmtlist }
    Token tok = lexer->peek();
    if(tok.type != TokenType::lbrace) {
        parse_error(PET::BadBlockStart, tok);
    }
    //consume {
    lexer->lex();
    parseStatementList(lexer);
    tok = lexer->peek();
    if(tok.type == TokenType::eof) {
        parse_error(PET::IncompleteBlock,tok);
    }
    //consume }
    lexer->lex();
}

void parseStatementList(LexerTarget* lexer) {
    //stmtlist -> stmt stmtlisttail
    //stmtlisttail -> stmtlist | null
    parseStatement(lexer);
    std::cout << "Statement matched\n";
    Token tok = lexer->peek();
    if(tok.type != TokenType::rbrace) {
        parseStatementList(lexer);
    }
}

void parseStatement(LexerTarget* lexer) {
    //stmt -> deferblock | ifblock | loop | block | returnstmt
    //     -> varstmt ; | expr ; | ;
    Token tok = lexer->peek();
    if(tok.type == TokenType::semicolon) {
        //consume ;
        lexer->lex();
        return;
    } else if(tok.type == TokenType::sif) {
        parseIfblock(lexer);
    } else if(tok.type == TokenType::sfor || tok.type == TokenType::swhile) {
        parseLoop(lexer);
    } else if(tok.type == TokenType::sdefer) {
        parseDeferBlock(lexer);
    } else if(tok.type == TokenType::lbrace) {
        parseBlock(lexer);
    } else if(tok.type == TokenType::sreturn) {
        parseReturnStatement(lexer);
    } else {
        parseExpression(lexer);
        //consume ;
        lexer->lex();
    }
}

void parseStatementListLoop(LexerTarget* lexer) {
/* stmtloop -> stmt | flowctrl
 * stmtlistloop -> stmtloop  stmtlistlooptail
 * stmtlistlooptail -> stmtlistloop | null
 * flowctrl -> break ; | continue ;
 */
    //TODO(marcus): implement this function.
    std::cout << "DON'T CALL ME!!!\n";
}

void parseIfblock(LexerTarget* lexer) {
/*
 * ifblock -> . if ( expression ) ifelsebody optelseblock
 */
    //consume if
    Token tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissIfLParen, tok);
    }
    //consume (
    lexer->lex();
    parseExpression(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissIfRParen, tok);
    }
    //consume )
    lexer->lex();
    parseIfElseBody(lexer);
    parseOptElseBlock(lexer);
}

void parseIfElseBody(LexerTarget* lexer) {
    /*
     * ifelsebody -> block | stmt ;
     */
    //May not need to actually lex here, just call parsestatement() again...
    Token tok = lexer->peek();
    //Double check for error with peek/lex below. (rewrite earlier functions?)
    if(tok.type == TokenType::lbrace) {
        parseBlock(lexer);
    } else {
        parseStatement(lexer);
    }
}

void parseOptElseBlock(LexerTarget* lexer) {
    /*
     * optelseblock -> . else ifelsebody | null
     */
    Token tok = lexer->peek();
    if(tok.type != TokenType::selse) {
        return;
    }
    //consume else
    lexer->lex();
    parseIfElseBody(lexer);
}

void parseLoop(LexerTarget* lexer) {
    /*
     * loop -> . loophead { stmtlistloop }
     * loophead -> for ( vardecassign ; conditional ; expr ) | while ( expression )
     */
    Token tok = lexer->peek();
    if(tok.type == TokenType::sfor) {
        parseForLoop(lexer);
    } else if(tok.type == TokenType::swhile) {
        parseWhileLoop(lexer);
    } else {
        parse_error(PET::Unknown, tok);
    }
    tok = lexer->peek();
    if(tok.type != TokenType::lbrace) {
        parse_error(PET::Unknown, tok);
    }
    //consume {
    lexer->lex();
    parseStatementListLoop(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rbrace) {
        parse_error(PET::Unknown, tok);
    }
    //consume }
    lexer->lex();
}

void parseForLoop(LexerTarget* lexer) {
    //forloop -> . for ( vardecassign ; conditional ; expr )
    //consume for
    Token tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissLParenFor, tok);
    }
    //consume (
    lexer->lex();
    parseVarDecAssign(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissSemicolonFor1, tok);
    }
    //consume ;
    lexer->lex();
    parseExpression(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissSemicolonFor2, tok);
    }
    //consume ;
    lexer->lex();
    parseExpression(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissRParenFor, tok);
    }
    //consume )
    lexer->lex();
}

void parseWhileLoop(LexerTarget* lexer) {
    //whileloop -> . while ( expression )
    //consume while
    Token tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissLParenWhile, tok);
    }
    //consume (
    lexer->lex();
    parseExpression(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissRParenWhile, tok);
    }
    //consume )
    lexer->lex();
}

void parseDeferBlock(LexerTarget* lexer) {
    //deferblock -> . defer stmt
    //consume defer
    lexer->lex();
    parseStatement(lexer);
}
void parseReturnStatement(LexerTarget* lexer) {
    /*
     * returnstmt -> . return expr ;
     * returnstmt -> . return ;
     */
    //consume return
    Token tok = lexer->lex();
    if(tok.type == TokenType::semicolon) {
        //consume ;
        lexer->lex();
        return;
    }
    parseExpression(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissSemiReturn, tok);
    }
    //consume ;
    lexer->lex();
    return;
}

void parseExpression(LexerTarget* lexer) {
/* 
 * expr -> multdiv plusmin expr  | multdiv
 */
    //Token tok = lexer->peek();
    parseMultdiv(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::plus || tok.type == TokenType::minus) {
        //don't consume as we already do above
        parseExpression(lexer);
    }
    return;
}

void parseMultdiv(LexerTarget* lexer) {
 /* 
  * multdiv -> parenexp starslash multdiv | parenexp
  */
    parseParenexp(lexer);
    Token tok = lexer->peek();
    if(tok.type == TokenType::star || tok.type == TokenType::fslash) {
        //consume token
        lexer->lex();
        parseMultdiv(lexer);
    }
    return;
}

void parseParenexp(LexerTarget* lexer) {
 /* 
  * parenexp -> ( expr ) | const | var | funcall
  */
    Token tok = lexer->peek();
    if(tok.type == TokenType::lparen) {
        //consume (
        lexer->lex();
        parseExpression(lexer);
        tok = lexer->peek();
        if(tok.type != TokenType::rparen) {
            parse_error(PET::Unknown, tok);
        }
        //consume )
        lexer->lex();
        return;
    } else if(tok.type == TokenType::intlit || tok.type == TokenType::floatlit) {
        parseConst(lexer);
    } else if(tok.type == TokenType::id) {
        parseFunccallOrVar(lexer);
    } else {
        parse_error(PET::Unknown, tok);
    }
    return;
}

void parseConst(LexerTarget* lexer) {
    // const -> ilit | flit | charlit
    //Consume token
    lexer->lex();
    return;
}

void parseFunccallOrVar(LexerTarget* lexer) {
    //get id
    Token tok = lexer->peek();
    //consume id, get potential (
    Token tokNext = lexer->lex();
    if(tokNext.type == TokenType::lparen) {
        parseFunccall(lexer);
    } else {
        //Will not work, have to fix somehow...
        //pass token into parseVar?
        //parseVar(lexer);    
        //for now just manually check if it is an id
        if(tok.type != TokenType::id) {
            //unneeded check? do we already know its an id?
            parse_error(PET::BadVarName, tok);
        }
    }
}

void parseFunccall(LexerTarget* lexer) {
    //funccall -> funcname . ( opt_args )
    // funcname -> id
    Token tok = lexer->peek();
    if(tok.type != TokenType::lparen) {
        std::cout << __FUNCTION__ << ": token wasn't (, was " << tok.token << '\n';
        parse_error(PET::Unknown, tok);
    }
    //consume (
    tok = lexer->lex();
    if(tok.type != TokenType::rparen) {
        parseOptargs(lexer);
    }
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        std::cout << __FUNCTION__ << ": token wasn't ), was " << tok.token << '\n';
        parse_error(PET::Unknown, tok);
    }
    //consume ')'
    lexer->lex();
    return;
}

void parseOptargs(LexerTarget* lexer) {
    //opt_args -> null | id . | id . , opt_args2
    Token tok = lexer->peek();
    if(tok.type == TokenType::rparen) {
        return;
    }
    //TODO(marcus): This should accept any expression. Should simplify code.
    if(tok.type != TokenType::id && tok.type != TokenType::intlit) {
        parse_error(PET::Unknown, tok);
    }
    //consume id
    tok = lexer->lex();
    if(tok.type == TokenType::comma) {
        //consume ,
        lexer->lex();
        parseOptargs2(lexer);
    }
    //Else return. if it is a ')' we will catch it in parseFunccall
    return;
}

void parseOptargs2(LexerTarget* lexer) {
    //opt_args2 -> . id | . id , opt_args2
    Token tok = lexer->peek();
    //TODO(marcus): This should accept any expression. Should simplify code.
    if(tok.type != TokenType::id) {
        parse_error(PET::Unknown, tok);
    }
    //consume id
    tok = lexer->lex();
    if(tok.type == TokenType::comma) {
        //consume ,
        lexer->lex();
        parseOptargs2(lexer);
    }
    //Else return. if it is a ')' we will catch it in parseFunccall
    return;
}
