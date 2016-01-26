#include <iostream>
#include <vector>
#include "parser.h"
#include "tokens.h"

//ParseErrorType:: is a pain to write out every time... :)
#define PET ParseErrorType

void parseTopLevelStatements(LexerTarget* lexer);
void parseImportStatement(LexerTarget* lexer);
void parseImportId(LexerTarget* lexer);
void parsePrototypeBegin(LexerTarget* lexer);
void parsePrototypeId(LexerTarget* lexer);
void parsePrototypeRParen(LexerTarget* lexer);
void parseOptparams(LexerTarget* lexer);
void parseOptparamsColon(LexerTarget* lexer);
void parseOptparamsTail(LexerTarget* lexer);
void parseType(LexerTarget* lexer);

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

//TODO(marcus) add comments for each function indicating where in a rule we are
void parseTopLevelStatements(LexerTarget* lexer) {
    Token tok = lexer->lex();
    if(tok.type == TokenType::import) {
        std::cout << "import token, beginning to match import statement...\n";
        parseImportStatement(lexer);
        std::cout << "import statement matched\n";
    } else if(tok.type == TokenType::fn) {
        //parseFunctionDef(lexer, &tok);
        std::cout << "Function Definitions Parse (Coming Soon)\n";
        //std::cout << "Token: " << tok.token << "\n";
        //std::cout << "TokenType: " << static_cast<std::underlying_type<TokenType>::type>(tok.type) << "\n";
        //TokenType t = TokenType::fn;
        //std::cout << "TokenType fn: " << static_cast<std::underlying_type<TokenType>::type>(t) << "\n";
    } else if(tok.type == TokenType::foreign) {
        std::cout << "extern token, beginning to match prototype...\n";
        parsePrototypeBegin(lexer);
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
    Token tok = lexer->lex();
    if(tok.type == TokenType::id) {
        parseImportId(lexer);
    } else {
        parse_error(ParseErrorType::BadImportName, tok);
    }
}

void parseImportId(LexerTarget* lexer) {
    Token tok = lexer->lex();
    if(tok.type != TokenType::semicolon) {
        parse_error(ParseErrorType::MissImportSemicolon, tok);
    }
    return;
}

void parsePrototypeBegin(LexerTarget* lexer) {
//prototypes -> extern fn id ( opt_args ) : type ;
    Token tok = lexer->lex();
    if(tok.type != TokenType::fn) {
        parse_error(PET::MissPrototypeFn, tok);
    }
    tok = lexer->lex();
    if(tok.type == TokenType::id) {
        parsePrototypeId(lexer);
    } else {
        parse_error(ParseErrorType::BadPrototypeName, tok);
    }
}

void parsePrototypeId(LexerTarget* lexer) {
    Token tok = lexer->lex();
    if(tok.type == TokenType::lparen) {
        parseOptparams(lexer);
    } else {
        parse_error(ParseErrorType::MissPrototypeLParen, tok);
    }
}

void parsePrototypeRParen(LexerTarget* lexer) {
    Token tok = lexer->lex();
    if(tok.type == TokenType::colon) {
        parseType(lexer);
        tok = lexer->lex();
        if(tok.type != TokenType::semicolon) {
            parse_error(PET::MissPrototypeSemicolon, tok);
        }
    } else {
        parse_error(ParseErrorType::MissPrototypeColon, tok);
    }
}

//opt_args -> null | id : type opt_args_tail
//opt_args_tail -> null | , opt_args
void parseOptparams(LexerTarget* lexer) {
    Token tok = lexer->lex();
    if(tok.type == TokenType::id) {
        parseOptparamsColon(lexer);
    } else if(tok.type == TokenType::rparen) {
        //FIXME(marcus): Opt_params are also present in function definitions, not just prototypes
        parsePrototypeRParen(lexer);
    } else {
        parse_error(ParseErrorType::BadFunctionParameter, tok);
    }
}

void parseOptparamsColon(LexerTarget* lexer) {
    Token tok = lexer->lex();
    if(tok.type == TokenType::colon) {
        parseType(lexer);
        parseOptparamsTail(lexer);
    } else {
        parse_error(PET::MissOptparamColon, tok);
    }
}

void parseOptparamsTail(LexerTarget* lexer) {
    Token tok = lexer->lex();
    if(tok.type == TokenType::comma) {
        parseOptparams(lexer);
    } else if(tok.type == TokenType::rparen) {
        //TODO(marcus): opt_params can be found in function definitions too.
        parsePrototypeRParen(lexer);
    } else {
        parse_error(PET::BadOptparamTail, tok);
    }
}

void parseType(LexerTarget* lexer) {
    Token tok = lexer->lex();
    //TODO(marcus): add keywords for primitive types
    if(tok.type == TokenType::id) {
        return;
    } else {
        parse_error(PET::BadTypeIdentifier, tok);
    }
}
