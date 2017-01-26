#ifndef REGEXES_H
#define REGEXES_H

#include <regex>
#include <utility>
#include "tokens.h"

extern std::pair<std::regex,TokenType> regexes[];
extern std::pair<std::regex,TokenType> number_literal_regexes[];
extern std::pair<std::regex,TokenType> operator_regexes[];
extern std::regex comment_line_regex;
extern std::regex comment_block_regex;
extern unsigned int num_regexes;
extern unsigned int num_number_literal_regexes;
extern unsigned int num_operator_regexes;
extern std::pair<std::regex,TokenType> str_lit_pair;
#endif
