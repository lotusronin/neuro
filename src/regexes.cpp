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
    std::make_pair(std::regex("void"),TokenType::tvoid),
    std::make_pair(std::regex("u8"),TokenType::tuchar),
    std::make_pair(std::regex("s8"),TokenType::tchar),
    std::make_pair(std::regex("u32"),TokenType::tuint),
    std::make_pair(std::regex("s32"),TokenType::tint),
    std::make_pair(std::regex("f32"),TokenType::tfloat),
    std::make_pair(std::regex("f64"),TokenType::tdouble),
    std::make_pair(std::regex("if"),TokenType::sif),
    std::make_pair(std::regex("for"),TokenType::sfor),
    std::make_pair(std::regex("while"),TokenType::swhile),
    std::make_pair(std::regex("defer"),TokenType::sdefer),
    std::make_pair(std::regex("return"),TokenType::sreturn),
    std::make_pair(std::regex("else"),TokenType::selse),
    std::make_pair(std::regex("break"),TokenType::sbreak),
    std::make_pair(std::regex("continue"),TokenType::scontinue),
    std::make_pair(std::regex("struct"),TokenType::tstruct),
    std::make_pair(std::regex("[a-zA-Z_][a-zA-Z0-9_]*"),TokenType::id)
};

std::pair<std::regex,TokenType> number_literal_regexes[] = { 
    std::make_pair(std::regex("[0-9]+\\.[0-9]*"),TokenType::floatlit),
    std::make_pair(std::regex("[0-9]+"),TokenType::intlit)
};

std::pair<std::regex,TokenType> operator_regexes[] = {
    std::make_pair(std::regex("="),TokenType::assignment),
    std::make_pair(std::regex("=="),TokenType::equality),
    std::make_pair(std::regex("!="),TokenType::nequality),
    std::make_pair(std::regex("<"),TokenType::lessthan),
    std::make_pair(std::regex("<="),TokenType::ltequal),
    std::make_pair(std::regex(">"),TokenType::greaterthan),
    std::make_pair(std::regex(">="),TokenType::gtequal),
    std::make_pair(std::regex("\\+"),TokenType::plus),
    std::make_pair(std::regex("-"),TokenType::minus),
    std::make_pair(std::regex("\\*"),TokenType::star),
    std::make_pair(std::regex("/"),TokenType::fslash),
    std::make_pair(std::regex("%"),TokenType::mod),
    std::make_pair(std::regex("!"),TokenType::exclaim),
    std::make_pair(std::regex("\\?"),TokenType::question),
    std::make_pair(std::regex("&"),TokenType::ampersand),
    std::make_pair(std::regex("&&"),TokenType::dblampersand),
    std::make_pair(std::regex("\\|"),TokenType::bar),
    std::make_pair(std::regex("\\|\\|"),TokenType::dblbar),
    std::make_pair(std::regex("\\^"),TokenType::carrot)
    ,std::make_pair(std::regex("~"),TokenType::tilda)
};

std::regex comment_line_regex = std::regex("^//");
std::regex comment_block_regex = std::regex("^/\\*");
/*
    std::make_pair(std::regex("\\("),TokenType::lparen),
    std::make_pair(std::regex("\\)"),TokenType::rparen),
    std::make_pair(std::regex("\\{"),TokenType::lbrace),
    std::make_pair(std::regex("\\}"),TokenType::rbrace),
    std::make_pair(std::regex("\\."),TokenType::dot),
    std::make_pair(std::regex("\\,"),TokenType::comma),
    std::make_pair(std::regex(";"),TokenType::semicolon),
    std::make_pair(std::regex(":"),TokenType::colon),
    std::make_pair(std::regex("@"),TokenType::dereference)
    
    //already check for comments earlier in lexer
    std::make_pair(std::regex("/\\*"),TokenType::comment),
    std::make_pair(std::regex("//.*"),TokenType::comment),
*/
unsigned int num_regexes = sizeof(regexes)/sizeof(std::pair<std::regex,TokenType>);
unsigned int num_number_literal_regexes = sizeof(number_literal_regexes)/sizeof(std::pair<std::regex,TokenType>);
unsigned int num_operator_regexes = sizeof(operator_regexes)/sizeof(std::pair<std::regex,TokenType>);

std::pair<std::regex,TokenType> str_lit_pair = std::make_pair(std::regex("\"[^\"]*\""),TokenType::strlit);
