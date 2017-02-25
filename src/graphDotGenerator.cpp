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
                id = ((CompileUnitNode*)ast)->id;
                outfile << "CompilationUnit"<<id<<";\n";
                outfile << "CompilationUnit"<<id<<"[label=\"" << ((CompileUnitNode*)ast)->mname << "\"];\n";
                header = "CompilationUnit";
            }
            break;
        case ANT::Prototype:
            {
                id = ((PrototypeNode*)ast)->id;
                outfile << "prototype" << id << ";\n";
                outfile << "prototype" << id << "[label=\"extern fn "<<((PrototypeNode*)ast)->mfuncname<<"\"];\n";
                header = "prototype";
            }
            break;
        case ANT::Params:
            {
                id = ((ParamsNode*)ast)->id;
                outfile << "param"<<id<<";\n";
                outfile << "param"<<id<<"[label=\""<<((ParamsNode*)ast)->mname<<"\"];\n";
                header = "param";
            }
            break;
        case ANT::Var:
            {
                id = ((VarNode*)ast)->id;
                outfile << "var"<<id<<";\n";
                outfile << "var"<<id<<"[label=\""<<((VarNode*)ast)->mname<<"\"];\n";
                header = "var";
            }
            break;
        case ANT::VarDec:
            {
                id = ((VarDecNode*)ast)->id;
                outfile << "vardec" << id << ";\n";
                outfile << "vardec" << id << "[label=\"vardec\"];\n";
                header = "vardec";
            }
            break;
        case ANT::VarDecAssign:
            {
                id = ((VarDecAssignNode*)ast)->id;
                outfile << "vardecassign" << id << ";\n";
                outfile << "vardecassign" << id << "[label=\"vardecassign\"];\n";
                header = "vardecassign";
            }
            break;
        case ANT::FuncDef:
            {
                id = ((FuncDefNode*)ast)->id;
                outfile << "funcDef" << id << ";\n";
                outfile << "funcDef" << id << "[label=\"fn "<<((FuncDefNode*)ast)->mfuncname<<"\ntype: "<<((FuncDefNode*)ast)->mstype<<"\"];\n";
                header = "funcDef";
            }
            break;
        case ANT::StructDef:
            {
                id = ((StructDefNode*)ast)->id;
                outfile << "struct"<<id<<";\n";
                outfile << "struct"<<id<<"[label=\"struct " <<  ((StructDefNode*)ast)->ident << "\"];\n";
                header = "struct";
            }
            break;
        case ANT::Block:
            {
                id = ((BlockNode*)ast)->id;
                outfile << "block" << id << ";\n";
                outfile << "block" << id << "[label=\"block\"];\n";
                header = "block";
            }
            break;
        case ANT::IfStmt:
            {
                id = ((IfNode*)ast)->id;
                outfile << "if" << id << ";\n";
                outfile << "if" << id << "[label=\"if\"];\n";
                header = "if";
            }
            break;
        case ANT::ForLoop:
            {
                id = ((ForLoopNode*)ast)->id;
                outfile << "forloop" << id << ";\n";
                outfile << "forloop" << id << "[label=\"for\"];\n";
                header = "forloop";
            }
            break;
        case ANT::DeferStmt:
            {
                id = ((DeferStmtNode*)ast)->id;
                outfile << "defer"<<id<<";\n";
                outfile << "defer"<<id<<"[label=\"defer\"];\n";
                header = "defer";
            }
            break;
        case ANT::WhileLoop:
            {
                id = ((WhileLoopNode*)ast)->id;
                outfile << "whileloop" << id << ";\n";
                outfile << "whileloop" << id << "[label=\"while\"];\n";
                header = "whileloop";
            }
            break;
        case ANT::RetStmnt:
            {
                id = ((ReturnNode*)ast)->id;
                outfile << "return" << id << ";\n";
                outfile << "return" << id << "[label=\"return\"];\n";
                header = "return";
            }
            break;
        case ANT::BinOp:
            {
                id = ((BinOpNode*)ast)->id;
                outfile << "binop" << id << ";\n";
                outfile << "binop" << id << "[label=\"" << ((BinOpNode*)ast)->mop << "\ntype: " << ((BinOpNode*)ast)->mstype << "\"];\n";
                header =  "binop";
            }
            break;
        case ANT::FuncCall:
            {
                id = ((FuncCallNode*)ast)->id;
                outfile << "funcCall" << id << ";\n";
                outfile << "funcCall" << id << "[label=\"" << ((FuncCallNode*)ast)->mfuncname << "()\"];\n";
                header = "funcCall";
            }
            break;
        case ANT::Const:
            {
                id = ((ConstantNode*)ast)->id;
                outfile << "constant" << id << ";\n";
                outfile << "constant" << id << "[label=\"" << ((ConstantNode*)ast)->mval << "\ntype: " << ((ConstantNode*)ast)->mstype << "\"];\n";
                header = "constant";
            }
            break;
        case ANT::LoopStmt:
            {
                id = ((LoopStmtNode*)ast)->id;
                outfile << "loopStmt" << id << ";\n";
                if(((LoopStmtNode*)ast)->isBreak) {
                    outfile << "loopStmt" << id << "[label=\"break\"];\n";
                } else {
                    outfile << "loopStmt" << id << "[label=\"continue\"];\n";
                }
                header = "loopStmt";
            }
            break;
        case ANT::Assign:
            {
                id = ((AssignNode*)ast)->id;
                outfile << "assign" << id << ";\n";
                outfile << "assign" << id << "[label=\"=\"];\n";
                header = "assign";
            }
            break;
        case ANT::Cast:
            {
                id = ((CastNode*)ast)->id;
                outfile << "cast"<<id<<";\n";
                outfile << "cast"<<id<<"[label=\"cast to " << ((CastNode*)ast)->toType << "\"];\n";
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
