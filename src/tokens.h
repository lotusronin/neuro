#ifndef TOKENS_H
#define TOKENS_H

#include <string>

enum class TokenType {
    fn,
    foreign,
    import,
    id,
    floatlit,
    intlit,
    assignment,
    equality,
    nequality,
    lessthan,
    ltequal,
    greaterthan,
    gtequal,
    lparen,
    rparen,
    lbrace,
    rbrace,
    dot,
    comma,
    plus,
    minus,
    star,
    fslash,
    semicolon,
    colon,
    exclaim,
    comment,
    question,
    eof
};

struct Token {
    TokenType type;
    unsigned int col;
    unsigned int line;
    std::string token;
};

#endif