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
void parseFunctionDef(LexerTarget* lexer);
void parseBlock(LexerTarget* lexer);
void parseStatementList(LexerTarget* lexer) {

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
    parseTopLevelStatements(mlexer);
}

void parseTopLevelStatements(LexerTarget* lexer) {
    //tl_statements -> imports tl_statements 
    //tl_statements -> prototypes tl_statements
    //tl_statements -> functiondefs tl_statements 
    //tl_statements -> null
    Token tok = lexer->lex();
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
        std::cout << "File is parsed, no errors detected!\n";
        return;
    }else {
        parse_error(ParseErrorType::BadTopLevelStatement, tok);
    }
    parseTopLevelStatements(lexer);
}

void parseImportStatement(LexerTarget* lexer) {
    //imports -> import . id ;
    Token tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadImportName, tok);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::semicolon) {
        parse_error(ParseErrorType::MissImportSemicolon, tok);
    }
    return;
}

void parsePrototype(LexerTarget* lexer) {
    //prototypes -> extern . fn id ( opt_params ) : type ;
    Token tok = lexer->lex();
    if(tok.type != TokenType::fn) {
        parse_error(PET::MissPrototypeFn, tok);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadPrototypeName, tok);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(ParseErrorType::MissPrototypeLParen, tok);
    }
    parseOptparams(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissPrototypeRParen, tok);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissPrototypeColon, tok);
    }
    parseType(lexer);
    tok = lexer->lex();
    if(tok.type != TokenType::semicolon) {
        parse_error(PET::MissPrototypeSemicolon, tok);
    }
}

void parseOptparams(LexerTarget* lexer) {
    //opt_args -> null | id : type opt_args_tail
    Token tok = lexer->lex();
    if(tok.type == TokenType::rparen) {
        return;
    }
    if(tok.type != TokenType::id) {
        parse_error(ParseErrorType::BadFunctionParameter, tok);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissOptparamColon, tok);
    }
    parseType(lexer);
    parseOptparamsTail(lexer);
}

void parseOptparamsTail(LexerTarget* lexer) {
    //opt_args_tail -> null | , opt_args
    Token tok = lexer->lex();
    if(tok.type == TokenType::comma) {
        parseOptparams(lexer);
    } else if(tok.type == TokenType::rparen) {
        return;
    } else {
        parse_error(PET::BadOptparamTail, tok);
    }
}

void parseType(LexerTarget* lexer) {
    //type -> int | char | float | double | bool | id
    Token tok = lexer->lex();
    if(tok.type == TokenType::tint) {
        return;
    } else if(tok.type == TokenType::tchar) {
        return;
    } else if(tok.type == TokenType::tbool) {
        return;
    } else if(tok.type == TokenType::tfloat) {
        return;
    } else if(tok.type == TokenType::tdouble) {
        return;
    } else if(tok.type == TokenType::id) {
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
    Token tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(PET::BadVarName, tok);
    }
    return;
}

void parseVarDec(LexerTarget* lexer) {
    //vardec -> var : type
    parseVar(lexer);
    Token tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissVardecColon, tok);
    }
    parseType(lexer);
    return;
}

void parseFunctionDef(LexerTarget* lexer) {
    //functiondefs -> fn id ( opt_params ) : type block
    Token tok = lexer->lex();
    if(tok.type != TokenType::id) {
        parse_error(PET::BadPrototypeName, tok);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::lparen) {
        parse_error(PET::MissPrototypeLParen,tok);
    }
    parseOptparams(lexer);
    tok = lexer->peek();
    if(tok.type != TokenType::rparen) {
        parse_error(PET::MissPrototypeRParen,tok);
    }
    tok = lexer->lex();
    if(tok.type != TokenType::colon) {
        parse_error(PET::MissPrototypeColon,tok);
    }
    parseType(lexer);
    parseBlock(lexer);
}

void parseBlock(LexerTarget* lexer) {
    //block -> { stmtlist }
    Token tok = lexer->lex();
    if(tok.type != TokenType::lbrace) {
        parse_error(PET::BadBlockStart, tok);
    }
    do {
        tok = lexer->lex();
        if(tok.type == TokenType::eof) {
            parse_error(PET::IncompleteBlock,tok);
        }
    } while(tok.type != TokenType::rbrace);
}

void parseStatementList(LexerTarget* lexer) {
    //stmtlist -> stmt stmtlisttail
}
