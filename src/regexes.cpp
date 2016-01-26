#include "regexes.h"

std::pair<std::regex,TokenType> regexes[] = { 
    std::make_pair(std::regex("fn"),TokenType::fn),
    std::make_pair(std::regex("extern"),TokenType::foreign),
    std::make_pair(std::regex("import"),TokenType::import),
    std::make_pair(std::regex("char"),TokenType::tchar),
    std::make_pair(std::regex("int"),TokenType::tint),
    std::make_pair(std::regex("bool"),TokenType::tbool),
    std::make_pair(std::regex("float"),TokenType::tfloat),
    std::make_pair(std::regex("double"),TokenType::tdouble),
    std::make_pair(std::regex("if"),TokenType::sif),
    std::make_pair(std::regex("for"),TokenType::sfor),
    std::make_pair(std::regex("while"),TokenType::swhile),
    std::make_pair(std::regex("defer"),TokenType::sdefer),
    std::make_pair(std::regex("return"),TokenType::sreturn),
    std::make_pair(std::regex("else"),TokenType::selse),
    std::make_pair(std::regex("/\\*"),TokenType::comment),
    std::make_pair(std::regex("//.*"),TokenType::comment),
    std::make_pair(std::regex("[a-zA-Z_][a-zA-Z0-9_]*"),TokenType::id),
    std::make_pair(std::regex("[0-9]+\\.[0-9]*"),TokenType::floatlit),
    std::make_pair(std::regex("[0-9]+"),TokenType::intlit),
    std::make_pair(std::regex("="),TokenType::assignment),
    std::make_pair(std::regex("=="),TokenType::equality),
    std::make_pair(std::regex("!="),TokenType::nequality),
    std::make_pair(std::regex("<"),TokenType::lessthan),
    std::make_pair(std::regex("<="),TokenType::ltequal),
    std::make_pair(std::regex(">"),TokenType::greaterthan),
    std::make_pair(std::regex(">="),TokenType::gtequal),
    std::make_pair(std::regex("\\("),TokenType::lparen),
    std::make_pair(std::regex("\\)"),TokenType::rparen),
    std::make_pair(std::regex("\\{"),TokenType::lbrace),
    std::make_pair(std::regex("\\}"),TokenType::rbrace),
    std::make_pair(std::regex("\\."),TokenType::dot),
    std::make_pair(std::regex("\\,"),TokenType::comma),
    std::make_pair(std::regex("\\+"),TokenType::plus),
    std::make_pair(std::regex("-"),TokenType::minus),
    std::make_pair(std::regex("\\*"),TokenType::star),
    std::make_pair(std::regex("/"),TokenType::fslash),
    std::make_pair(std::regex(";"),TokenType::semicolon),
    std::make_pair(std::regex(":"),TokenType::colon),
    std::make_pair(std::regex("!"),TokenType::exclaim),
    std::make_pair(std::regex("\\?"),TokenType::question)
};

unsigned int num_regexes = sizeof(regexes)/sizeof(std::pair<std::regex,TokenType>);


