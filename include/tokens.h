#ifndef TOKENS_H
#define TOKENS_H

#include <string>

enum class TokenType {
    fn,
    foreign,
    import,
    tchar,
    tuchar,
    tint,
    tuint,
    tbool,
    tfloat,
    tdouble,
    tvoid,
    tstruct,
    sif,
    sfor,
    swhile,
    sdefer,
    sreturn,
    selse,
    sbreak,
    scontinue,
    id,
    floatlit,
    intlit,
    strlit,
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
    ampersand,
    dblampersand,
    bar,
    dblbar,
    dereference,
    carrot,
    tilda,
    mod,
    eof
};

struct Token {
    TokenType type;
    unsigned int col;
    unsigned int line;
    //std::string token;
    char* token;
};

#endif
