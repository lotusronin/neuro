#include "astnodetypes.h"
#include <fstream>
#include <vector>

#define ANT AstNodeType

static void makeGraph(std::ofstream& outfile, AstNode* ast);

void makeDotGraph(std::ofstream& outfile, AstNode* ast) {
    outfile << "digraph Program {\n";
    makeGraph(outfile, ast);
    outfile << "}";
}

static void makeGraph(std::ofstream& outfile, AstNode* ast) {
    std::string header;
    int id;
    switch(ast->nodeType()) {
        case ANT::Program:
            {
                outfile << "Prog [label=\"Program\"];\n";
                header = "Prog";
                id = 0;
            }
            break;
        case ANT::CompileUnit:
            {
                auto compunit = static_cast<CompileUnitNode*>(ast);
                id = compunit->id;
                outfile << "CompilationUnit"<<id<<";\n";
                outfile << "CompilationUnit"<<id<<"[label=\"" << compunit->mname << "\"];\n";
                header = "CompilationUnit";
            }
            break;
        case ANT::Prototype:
            {
                auto proto = static_cast<FuncDefNode*>(ast);
                id = proto->id;
                outfile << "prototype" << id << ";\n";
                outfile << "prototype" << id << "[label=\"extern fn "<<proto->mfuncname<<"\"];\n";
                header = "prototype";
            }
            break;
        case ANT::Params:
            {
                auto param = static_cast<ParamsNode*>(ast);
                id = param->id;
                outfile << "param"<<id<<";\n";
                outfile << "param"<<id<<"[label=\""<<param->mname<<"\"];\n";
                header = "param";
            }
            break;
        case ANT::Var:
            {
                auto varn = static_cast<VarNode*>(ast);
                id = varn->id;
                outfile << "var"<<id<<";\n";
                outfile << "var"<<id<<"[label=\""<<varn->mname<<"\"];\n";
                header = "var";
            }
            break;
        case ANT::VarDec:
            {
                id = static_cast<VarDeclNode*>(ast)->id;
                outfile << "vardec" << id << ";\n";
                outfile << "vardec" << id << "[label=\"vardec\"];\n";
                header = "vardec";
            }
            break;
        case ANT::VarDecAssign:
            {
                id = static_cast<VarDeclNode*>(ast)->id;
                outfile << "vardecassign" << id << ";\n";
                outfile << "vardecassign" << id << "[label=\"vardecassign\"];\n";
                header = "vardecassign";
            }
            break;
        case ANT::FuncDef:
            {
                auto func = static_cast<FuncDefNode*>(ast);
                id = func->id;
                outfile << "funcDef" << id << ";\n";
                outfile << "funcDef" << id << "[label=\"fn "<<func->mfuncname<<"\ntype: "<<func->mtypeinfo.type<<"\"];\n";
                header = "funcDef";
            }
            break;
        case ANT::StructDef:
            {
                auto structn = static_cast<StructDefNode*>(ast);
                id = structn->id;
                outfile << "struct"<<id<<";\n";
                outfile << "struct"<<id<<"[label=\"struct " <<  structn->ident << "\"];\n";
                header = "struct";
            }
            break;
        case ANT::Block:
            {
                id = static_cast<BlockNode*>(ast)->id;
                outfile << "block" << id << ";\n";
                outfile << "block" << id << "[label=\"block\"];\n";
                header = "block";
            }
            break;
        case ANT::IfStmt:
            {
                id = static_cast<IfNode*>(ast)->id;
                outfile << "if" << id << ";\n";
                outfile << "if" << id << "[label=\"if\"];\n";
                header = "if";
            }
            break;
        case ANT::ForLoop:
            {
                id = static_cast<LoopNode*>(ast)->id;
                outfile << "forloop" << id << ";\n";
                outfile << "forloop" << id << "[label=\"for\"];\n";
                header = "forloop";
            }
            break;
        case ANT::DeferStmt:
            {
                id = static_cast<DeferStmtNode*>(ast)->id;
                outfile << "defer"<<id<<";\n";
                outfile << "defer"<<id<<"[label=\"defer\"];\n";
                header = "defer";
            }
            break;
        case ANT::WhileLoop:
            {
                id = static_cast<LoopNode*>(ast)->id;
                outfile << "whileloop" << id << ";\n";
                outfile << "whileloop" << id << "[label=\"while\"];\n";
                header = "whileloop";
            }
            break;
        case ANT::RetStmnt:
            {
                id = static_cast<ReturnNode*>(ast)->id;
                outfile << "return" << id << ";\n";
                outfile << "return" << id << "[label=\"return\"];\n";
                header = "return";
            }
            break;
        case ANT::BinOp:
            {
                auto binop = static_cast<BinOpNode*>(ast);
                id = binop->id;
                outfile << "binop" << id << ";\n";
                outfile << "binop" << id << "[label=\"" << binop->mop << "\ntype: " << binop->mtypeinfo.type << "\"];\n";
                header =  "binop";
            }
            break;
        case ANT::FuncCall:
            {
                auto call = static_cast<FuncCallNode*>(ast);
                id = call->id;
                outfile << "funcCall" << id << ";\n";
                outfile << "funcCall" << id << "[label=\"" << call->mfuncname << "()\"];\n";
                header = "funcCall";
            }
            break;
        case ANT::Const:
            {
                auto constn = static_cast<ConstantNode*>(ast);
                id = constn->id;
                outfile << "constant" << id << ";\n";
                outfile << "constant" << id << "[label=\"" << constn->mval << "\ntype: " << constn->mtypeinfo << "\"];\n";
                header = "constant";
            }
            break;
        case ANT::LoopStmt:
            {
                auto stmt = static_cast<LoopStmtNode*>(ast);
                id = stmt->id;
                outfile << "loopStmt" << id << ";\n";
                if(stmt->isBreak) {
                    outfile << "loopStmt" << id << "[label=\"break\"];\n";
                } else {
                    outfile << "loopStmt" << id << "[label=\"continue\"];\n";
                }
                header = "loopStmt";
            }
            break;
        case ANT::Assign:
            {
                id = static_cast<AssignNode*>(ast)->id;
                outfile << "assign" << id << ";\n";
                outfile << "assign" << id << "[label=\"=\"];\n";
                header = "assign";
            }
            break;
        case ANT::Cast:
            {
                auto cast = static_cast<CastNode*>(ast);
                id = cast->id;
                outfile << "cast"<<id<<";\n";
                outfile << "cast"<<id<<"[label=\"cast to " << cast->mtypeinfo << "\"];\n";
                header = "cast";
            }
            break;
        default:
            break;
    }
    
    for (auto child : *(ast->getChildren())) {
        outfile << header << id << " -> ";
        makeGraph(outfile, child);
    }
}

#undef ANT
