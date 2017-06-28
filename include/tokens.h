#ifndef TOKENS_H
#define TOKENS_H

#include <string>

enum class TokenType {
    fn,
    foreign,
    import,
    tchar,
    tuchar,
    tshort,
    tushort,
    tint,
    tuint,
    tlongint,
    tulongint,
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
    charlit,
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
    cast,
    ssizeof,
    increment,
    decrement,
    dblcolon,
    lsqrbrace,
    rsqrbrace,
    addassign,
    subassign,
    mulassign,
    divassign,
    eof
};

struct Token {
    TokenType type;
    unsigned int col;
    unsigned int line;
    //std::string token;
    const char* token;
};

#endif
