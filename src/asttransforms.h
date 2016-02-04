#ifndef ASTTRANSFORMS_H
#define ASTTRANSFORMS_H
#include "astnode.h"

void collapseExpressionChains(AstNode* ast);
void checkContinueBreak(AstNode* ast, int loopDepth);
void fixOperatorAssociativity(AstNode* ast);

#endif
