%{
#include <string>
#include "node.h"
#include "parser.h"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}
%x COMMENT
%%
%{
int comment_depth = 0;
%}
[ \t\n]                     ;
"fn"                        return TOKEN(TFN);
"extern"                    return TOKEN(TEXTERN);
[a-zA-Z_][a-zA-Z0-9_]*      SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]*              SAVE_TOKEN; return TDOUBLE;
[0-9]+                      SAVE_TOKEN; return TINTEGER;
"="                         return TOKEN(TEQUAL);
"=="                        return TOKEN(TCEQ);
"!="                        return TOKEN(TCNE);
"<"                         return TOKEN(TCLT);
"<="                        return TOKEN(TCLE);
">"                         return TOKEN(TCGT);
">="                        return TOKEN(TCGE);
"("                         return TOKEN(TLPAREN);
")"                         return TOKEN(TRPAREN);
"{"                         return TOKEN(TLBRACE);
"}"                         return TOKEN(TRBRACE);
"."                         return TOKEN(TDOT);
","                         return TOKEN(TCOMMA);
"+"                         return TOKEN(TPLUS);
"-"                         return TOKEN(TMINUS);
"*"                         return TOKEN(TMUL);
"/"                         return TOKEN(TDIV);
";"                         return TOKEN(TSEMI);
":"                         return TOKEN(TCOLON);
"->"                        return TOKEN(TARROW);

<COMMENT,INITIAL>"/*"       printf("Comment depth %d!\n",comment_depth+1); BEGIN(COMMENT); comment_depth++; 
<COMMENT>[^*/]*             ;
<COMMENT>"*/"               comment_depth--; if (comment_depth == 0) BEGIN(INITIAL); printf("Comment depth %d\n",comment_depth);
.                           printf("Unknown token!\n"); yyterminate();

%%
