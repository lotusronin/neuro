#ifndef REGEXES_H
#define REGEXES_H

#include <regex>
#include <utility>
#include "tokens.h"

extern std::pair<std::regex,TokenType> regexes[];
extern unsigned int num_regexes;

#endif
