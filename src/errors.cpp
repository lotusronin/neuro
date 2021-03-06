#include "errors.h"
#include "coloroutput.h"
#include "astnode.h"
#include "symboltable.h"
#include "astnodetypes.h"
#include "lexer.h"
#include <iostream>
#include <fstream>
#include <string>

void printErrorContext(int line, int col, const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    in.seekg(0,std::ios::end);
    int size = in.tellg();
    char* content = static_cast<char*>(malloc(size+1));
    in.seekg(0,std::ios::beg);
    in.read(content,size);
    content[size] = '\0';

    int tmp_l = 0;
    int tmp_c = 0;
    char* ptr = content;
    while(tmp_l != line) {
        char t = *ptr++;
        tmp_c++;
        if((t == '\n') || (t == '\r')) {
            tmp_c = 0;
            tmp_l++;
        }
    }

    std::cout << "\n" << ANSI_RED << "<Error> " << ANSI_CLEAR << filename << ':' << line << ':' << col <<  "\n";
    char t = *ptr;
    while((t != '\0') && (t != '\n') && (t != '\r')) {
        std::cout << *ptr;
        ptr++;
        t = *ptr;
    }
    std::cout << '\n';

    if(col > 0) {
        for(int i = 0; i < col; i++) {
            std::cout << '-';
        }
    }
    std::cout << "^\n";

    free(content);
    return;
}


#define PET ParseErrorType
int parse_error(const ParseErrorType type, const Token& t, const LexerTarget* l) {
    printErrorContext(t.line,t.col,l->targetName());
    switch (type) {
        case ParseErrorType::BadTopLevelStatement:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad Top Level Statement\n";
            std::cout << "  Token: " << t.token << " Is not a valid start of a top level statement\n";
            break;
        case ParseErrorType::BadImportName:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Invalid import value\n";
            std::cout << "  Token: " << t.token << " Cannot be imported\n";
            break;
        case ParseErrorType::MissPrototypeFn:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expecting fn keyword\n";
            std::cout << "  Token: " << t.token << " is not 'fn'!\n";
            break;
        case ParseErrorType::BadPrototypeName:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad prototype identifier\n";
            std::cout << "  Token: " << t.token << " is not a valid prototype name!\n";
            break;
        case ParseErrorType::BadFunctionParameter:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad function parameter\n";
            std::cout << "  Token: " << t.token << " Expected an identifier or ')'!\n";
            break;
        case ParseErrorType::BadTypeIdentifier:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad type identifier\n";
            std::cout << "  Token: " << t.token << " is not a type!\n";
            break;
        case ParseErrorType::BadOptparamTail:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Bad parameter formation\n";
            std::cout << "  Token: " << t.token << " Expected a ',' or a ')'!\n";
            break;
        case ParseErrorType::BadVarName:
            std::cout << "File:" << t.line << ":" << t.col << " Error: bad variable name\n";
            std::cout << "  Token: " << t.token << " is not a valid variable identifier!\n";
            break;
        case ParseErrorType::IncompleteBlock:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Block was never closed\n";
            std::cout << "  Token: " << t.token << " is not a '}'!\n";
            break;
        case ParseErrorType::MissEqVarDecAssign:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Malformed variable declaration assignment\n";
            std::cout << "  Token: " << t.token << " is not a '='!\n";
            break;
        case ParseErrorType::MissForeign:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expected function or struct declaration\n";
            std::cout << "  Token: " << t.token << " is not 'fn' or 'struct'!\n";
            break;
        case ParseErrorType::MultipleScope:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Multiple scope resolutions for an expression\n";
            std::cout << "  Token: " << t.token << " is not a valid identifier. Can only have one level of scope resolution.\n";
            break;
        case ParseErrorType::MissLParen:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expected Left paren\n";
            std::cout << "  Token: " << t.token << " is not a '('\n";
            break;
        case ParseErrorType::MissRParen:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expected Right paren\n";
            std::cout << "  Token: " << t.token << " is not a ')'\n";
            break;
        case ParseErrorType::MissSemicolon:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expected semicolon\n";
            std::cout << "  Token: " << t.token << " is not a ';'\n";
            break;
        case ParseErrorType::MissColon:
            std::cout << "File:" << t.line << ":" << t.col << " Error: Expected colon\n";
            std::cout << "  Token: " << t.token << " is not a ':'\n";
            break;
        case ParseErrorType::BadTypeModifier:
            std::cout << "File:"<< t.line << ":" << t.col << " Error: invalid type modifier\n";
            break;
       default:
            std::cout << "Unknown Parse Error!\n";
            break;
    }
    exit(-1);
    return 0;
}

#define SET SemanticErrorType
extern bool semantic_error;

void semanticError(const SemanticErrorType err, AstNode* n, const SymbolTable* s) {
    std::string file = "Unknown File";
    auto tmp_s = s;
    while(tmp_s->parent && (tmp_s->parent->name != "global")) {
        tmp_s = tmp_s->parent;
    }
    file = tmp_s->name;
    Token t;
    if(n) {
        t = n->mtoken;
    }
    printErrorContext(t.line,t.col,file);
    switch(err) {
        case SemanticErrorType::DotOpLhs:
        {
            ERROR("Error Left hand side of '.' is not valid!\n");
        }
        break;
        case SET::DupDecl:
        {
            ERROR("Error: Duplicate Declaration of variable: " << t.token << "!\n");
        }
        break;
        case SemanticErrorType::OutLoop:
        {
            ERROR("Error, Break or Continue used outside of a loop!\n");
        }
        break;
        case SemanticErrorType::NoFunc:
        {
            ERROR("Error: No Function named " << static_cast<FuncCallNode*>(n)->mfuncname << " is defined!\n");
        }
        break;
        case SET::MissmatchBinop:
        {
            auto binop = static_cast<BinOpNode*>(n);
            auto lt = binop->LHS()->mtypeinfo;
            auto rt = binop->RHS()->mtypeinfo;
            ERROR("Binary Operator had conflicting types: " << lt << " binop " << rt << '\n');
        }
        break;
        case SemanticErrorType::MissmatchReturnType:
        {
            auto retn = static_cast<ReturnNode*>(n);
            auto lt = retn->mchildren.at(0)->mtypeinfo;
            auto rt = retn->mtypeinfo;
            ERROR("Return value of type " << lt << " does not match function type " << rt << '\n');
        }
        break;
        case SemanticErrorType::MissmatchAssign:
        {
            auto assignn = static_cast<AssignNode*>(n);
            auto lt = assignn->getLHS()->mtypeinfo;
            auto rt = assignn->getRHS()->mtypeinfo;
            ERROR("Assign Type Error: Cannot assign " << rt << " to a variable of type " << lt << '\n');
        }
        break;
        case SemanticErrorType::MissmatchVarDecAssign:
        {
            auto vdassignn = static_cast<VarDeclNode*>(n);
            auto lt = vdassignn->getLHS()->mtypeinfo;
            auto rt = vdassignn->getRHS()->mtypeinfo;
            ERROR("Variable Decl and Assign Type Error: Cannot assign " << rt << " to a variable of type " << lt << '\n');
        }
        break;
        case SemanticErrorType::DerefNonPointer:
        {
            auto binop = static_cast<BinOpNode*>(n);
            auto lt = binop->LHS()->mtypeinfo;
            ERROR("Cannot Derefernece value, type " << lt << " is not a pointer\n");
        }
        break;
        case SemanticErrorType::NotLValue:
        {
            auto assignn = static_cast<AssignNode*>(n);
            auto name = assignn->getLHS()->mtoken.token;
            ERROR("Error, attempting to assign " << name << " to something that is not an lvalue!\n");
        }
        break;
        case SemanticErrorType::MissmatchFunctionParams:
        {
            //TODO(marcus): get expected type(s) for parameter
            //mayber just make this failed to find matching overload?
            //ERROR("Function parameter expected " << rt << " but was given " << lt << '\n');
        }
        break;
        case SemanticErrorType::UndefUse:
        {
            std::string name = static_cast<VarNode*>(n)->getVarName();
            ERROR("Error: Variable " << name << " used before it was defined!\n");
        }
        break;
        case SemanticErrorType::DupFuncDef:
        {
            ERROR("Duplicate defeinition of function " << static_cast<FuncDefNode*>(n)->mfuncname << "\n");
        }
        break;
        case SemanticErrorType::MultipleFuncResolve:
        {
            ERROR("Could not resolve function call of function " << static_cast<FuncCallNode*>(n)->mfuncname << "\n");
        }
        break;
        case SemanticErrorType::NoResolve:
        {
            ERROR("No function match found for " << static_cast<FuncCallNode*>(n)->mfuncname << "\n");
        }
        break;
        case SemanticErrorType::NoFunction:
        {
            ERROR("No function named " << static_cast<FuncCallNode*>(n)->mfuncname << " found\n");
        }
        break;
        case SemanticErrorType::VoidPtrDeref:
        {
            ERROR("Cannot dereference a pointer of type *void\n");
        }
        break;
        case SemanticErrorType::StructNotDefined:
        {
            auto vdassignn = static_cast<VarDeclNode*>(n);
            auto lt = vdassignn->getLHS()->mtypeinfo;
            ERROR("Struct type \"" << lt.userid << "\" has no definition\n");
        }
        break;
        case SemanticErrorType::NegationPtr:
        {
            ERROR("Cannot negate pointer\n");
        }
        break;
        case SemanticErrorType::Unknown:
        {
            ERROR("UNKNOWN ERROR TYPE");
        }
        default:
        //Let's hope we never get here or something is terribly wrong... :)
        std::cout << "DEFAULT CASE SEMANTIC ERROR!\n";
        break;
    }
    std::cout << "\n";
    semantic_error = true;
}

void semanticError(const SemanticErrorType err, const std::vector<StructDefNode*>& types) {
    if(err == SemanticErrorType::CyclicTypeDefinitions) {
        ERROR("<Error> Couldn't resolve dependencies with provided types\n");
        std::cout << "Unresolved Types: ";
        for(auto n : types) {
            std::cout << n->getIdent() << ' ';
        }
        std::cout << '\n';
    } else {
        std::cout << __FILE__ << ':' << __LINE__ << " Compiler error, unknown error type " << static_cast<int>(err) << '\n';
    }
    std::cout << "\n";
    semantic_error = true;
    exit(-1);
}

void semanticError(const SemanticErrorType err, const TypeInfo& type) {
    if(err == SemanticErrorType::TypeSizeAlignCalc) {
        ERROR("Cannot calculate size or alignment for: " << type << "\n");
    } else {
        std::cout << __FILE__ << ':' << __LINE__ << " Compiler error, unknown error type " << static_cast<int>(err) << '\n';
    }
    std::cout << "\n";
    semantic_error = true;
}
