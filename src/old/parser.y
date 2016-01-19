%{
    #include "node.h"
    NBlock* programBlock;
    NProgram neuroprogram;

    extern int yylex();
    void yyerror(const char* s) { printf("Error: %s\n",s); }
%}


%union {
    Node* node;
    NBlock* block;
    NExpression* expr;
    NStatement* stmt;
    NIdentifier* ident;
    NVariableDeclaration* var_dec;
    NFunctionPrototype* prototype;
    std::vector<NVariableDeclaration*>* varvec;
    std::vector<NExpression*>* exprvec;
    std::string* string;
    int token;
    }


/*
 * Terminal symbols (tokens).
 */
%token <string> TIDENTIFIER TINTEGER TDOUBLE TEXTERN TFN
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT TSEMI TCOLON TARROW
%token <token> TPLUS TMINUS TMUL TDIV



%type <ident> ident
%type <expr> numeric expr
%type <varvec> func_decl_args
%type <exprvec> call_args
/*%type <program> program*/
%type <block> stmts block tlvl_stmts
%type <stmt> stmt var_dec func_decl
%type <prototype> func_proto
%type <token> comparison relation arithmetic

%left TCEQ TCNE TCLT TCLE TCGT TCGE
%left TPLUS TMINUS
%left TMUL TDIV


%start program


%%

program : program func_proto tlvl_stmts {neuroprogram.prototypes.push_back($<prototype>2);
             neuroprogram.blocks.push_back($<block>3);}
             | program tlvl_stmts func_proto {neuroprogram.blocks.push_back($<block>2);
             neuroprogram.prototypes.push_back($<prototype>3);}
             | tlvl_stmts {neuroprogram.blocks.push_back($<block>1); }
             | func_proto {neuroprogram.prototypes.push_back($<prototype>1);}
             ;

tlvl_stmts : var_dec TSEMI {$$ = new NBlock(); $$->statements.push_back($1);}
           | func_decl {$$ = new NBlock(); $$->statements.push_back($<stmt>1);}
           | tlvl_stmts var_dec TSEMI {$1->statements.push_back($<stmt>2);}
           | tlvl_stmts func_decl {$1->statements.push_back($<stmt>2);}
           ;

stmts : stmt { $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
      | stmts stmt { $1->statements.push_back($<stmt>2); }
      ;

stmt : var_dec TSEMI
     | expr TSEMI { $$ = new NExpressionStatement(*$1); }
     ;

block : TLBRACE stmts TRBRACE { $$ = $2; }
      | TLBRACE TRBRACE { $$ = new NBlock(); }
      ;

var_dec : ident TCOLON ident { $$ = new NVariableDeclaration(*$3, *$1); }
        | ident TCOLON ident TEQUAL expr { $$ = new NVariableDeclaration(*$3, *$1, $5); }
        ;

func_proto : TEXTERN TFN ident TLPAREN func_decl_args TRPAREN TARROW ident TSEMI { $$ = new
           NFunctionPrototype(*$8, *$3, *$5); delete $5; }
           ;
func_decl : TFN ident TLPAREN func_decl_args TRPAREN TARROW ident block 
          { $$ = new NFunctionDeclaration(*$7, *$2, *$4, *$8); delete $4; }
          ;
func_decl_args : /*blank*/  { $$ = new VariableList(); }
               | var_dec { $$ = new VariableList(); $$->push_back($<var_dec>1); }
               | func_decl_args TCOMMA var_dec { $1->push_back($<var_dec>3); }
               ;

ident : TIDENTIFIER { $$ = new NIdentifier(*$1); delete $1; }
      ;

numeric : TINTEGER { $$ = new NInteger(atol($1->c_str())); delete $1; }
        | TDOUBLE { $$ = new NDouble(atof($1->c_str())); delete $1; }
        ;

expr : ident TEQUAL expr { $$ = new NAssignment(*$<ident>1, *$3); }
     | ident TLPAREN call_args TRPAREN { $$ = new NMethodCall(*$1, *$3); delete $3;}
     | ident { $<ident>$ = $1; }
     | numeric
     | expr comparison expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr relation expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr arithmetic expr { $$ = new NBinaryOperator(*$1, $2, *$3); }
     | TLPAREN expr TRPAREN { $$ = $2; }
     ;

call_args :/*blank*/ {$$ = new ExpressionList(); }
          | expr { $$ = new ExpressionList(); $$->push_back($1); }
          | call_args TCOMMA expr { $1->push_back($3); }
          ;

arithmetic : TPLUS | TMINUS | TMUL | TDIV
           ;

comparison : TCEQ | TCNE
           ;

relation : TCLT | TCLE | TCGT | TCGE
         ;

%%
