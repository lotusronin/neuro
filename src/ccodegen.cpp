#include "ccodegen.h"
#include <fstream>
#include <iostream>
#include <set>
#include <locale>
#include <cstring>

extern bool debug_cgen;
#define DEBUGC(a) if(debug_cgen){a}

#define ST SemanticType
bool isPrimitiveType(ST type) {
    bool ret = false;
    switch(type) {
        case ST::Bool:
        case ST::Int:
        case ST::Void:
        case ST::Char:
        case ST::Double:
        case ST::Float:
            ret = true;
        default:
            break;
    }
    return ret;
}

std::string typeToString(ST type, AstNode* n) {
    std::string ret;
    switch(type) {
        case ST::Bool:
            { ret = "bool"; }
            break;
        case ST::Int:
            { ret = "int"; }
            break;
        case ST::Void:
            { ret = "void"; }
            break;
        case ST::Char:
            { ret = "char"; }
            break;
        case ST::Double:
            { ret = "double"; }
            break;
        case ST::Float:
            { ret = "float"; }
            break;
        default:
            {     
                //TODO(marcus): should default to int, have explicit case for user defined types
                auto children = n->getChildren();
                //TODO(marcus): don't hardcode child access
                auto retnode = children->at(children->size()-2);
                auto typeToken = retnode->mtoken;
                ret = typeToken.token;
            }
            break;
    }
    return ret;
}
#undef ST

void genCFile(std::string filename, const std::vector<SymbolTableEntry*>& exported) {
    std::cout << "Generating C file\n";
    std::ofstream outfile(filename+"_neuro.h",std::ofstream::out);
    std::set<std::string> types;
    std::vector<std::vector<std::string>> funcs;

    for(auto& entry : exported) {
        std::vector<std::string> exported_func;
        
        auto n = entry->node;
        std::string retType = typeToString(entry->type, n);
        exported_func.push_back(retType);
        if(!isPrimitiveType(entry->type)) {
            types.insert(retType);
        }
        std::string funcname = static_cast<FuncDefNode*>(n)->mfuncname;
        exported_func.push_back(funcname);

    //std::vector<std::pair<SemanticType,AstNode*>> funcParams;
        auto funcparams = entry->funcParams;
        for(auto p : funcparams) {
            std::string param_type = typeToString(p.first.type, p.second);
            if(!isPrimitiveType(p.first.type)) {
                types.insert(param_type);
            }
            std::string param_name = static_cast<ParamsNode*>(p.second)->mname;
            exported_func.push_back(param_type);
            exported_func.push_back(param_name);
        }

        
        funcs.push_back(exported_func);
    }


    //output header guard
    std::string guard = filename+"_neuro_h";
    for(auto& c : guard) {
        std::toupper(c,std::locale());
    }
    outfile << "#ifndef " << guard << '\n';
    outfile << "#define " << guard << '\n';

    //output all types
    outfile << "//TYPE DEFINITONS\n";
    for(auto& t : types) {
        outfile << "typedef struct " << t << " " << t << ";\n";
    }
    
    //output all functions
    outfile << "\n//FUNCTION DEFINITIONS\n";
    for(auto& f : funcs) {
        //return type and funcname
        outfile << f.at(0) << " " << f.at(1) << "(";
        
        //paramters
        for(unsigned int i = 2; i < f.size(); i += 2) {
            outfile << f.at(i) << " " << f.at(i+1);
            if(i < f.size()-2) { outfile << ", "; }
        }

        outfile << ");\n";
    }

    //end header guard
    outfile << "#endif //" << guard;

    return;
}

void testGenCFile() {
    //TODO(marcus): make a test case for this!
}

static void getFunctionNodes(AstNode* ast, std::vector<FuncDefNode*>& funcs) {
    AstNodeType type = ast->nodeType();
    switch(type) {
        case AstNodeType::Program:
        case AstNodeType::CompileUnit:
            for(auto c : (*(ast->getChildren()))) {
                getFunctionNodes(c, funcs);
            }
            break;
        case AstNodeType::FuncDef:
        case AstNodeType::Prototype:
            funcs.push_back(static_cast<FuncDefNode*>(ast));
            break;
        default:
            break;
    }
    return;
}

static void getUserTypeNodes(AstNode* ast, std::vector<StructDefNode*>& types) {
    AstNodeType type = ast->nodeType();
    switch(type) {
        case AstNodeType::Program:
        case AstNodeType::CompileUnit:
            for(auto c : (*(ast->getChildren()))) {
                getUserTypeNodes(c, types);
            }
            break;
        case AstNodeType::UnionDef:
        case AstNodeType::StructDef:
            types.push_back(static_cast<StructDefNode*>(ast));
            break;
        default:
            break;
    }
    return;
}

std::string generateTypeString(TypeInfo& t) {
    std::string type;
    if(t.type == SemanticType::User) {
        type = t.userid;
    } else {
        switch(t.type) {
            case SemanticType::intlit:
                type = std::string("s32");
                break;
            case SemanticType::floatlit:
                type = std::string("f32");
                break;
            case SemanticType::u8:
                type = std::string("u8");
                break;
            case SemanticType::u16:
                type = std::string("u16");
                break;
            case SemanticType::u32:
                type = std::string("u32");
                break;
            case SemanticType::u64:
                type = std::string("u64");
                break;
            case SemanticType::s8:
                type = std::string("s8");
                break;
            case SemanticType::s16:
                type = std::string("s16");
                break;
            case SemanticType::s32:
                type = std::string("s32");
                break;
            case SemanticType::s64:
                type = std::string("s64");
                break;
            case SemanticType::Void:
                type = std::string("void");
                break;
            case SemanticType::Bool:
            case SemanticType::Int:
                type = std::string("int");
                break;
            case SemanticType::Float:
                type = std::string("f32");
                break;
            case SemanticType::Double:
                type = std::string("f64");
                break;
            case SemanticType::Char:
                type = std::string("char");
                break;
            default:
                type = std::string("unknown_t");
                break;
        }
    }

    int x = t.indirection;
    while(x) {
        type += "*";
        --x;
    }
    return type;
}

void printStruct(StructDefNode* n, std::ofstream& out) {
    const char* str_uni = n->nodeType() == (AstNodeType::StructDef) ? "struct " : "union ";
    if(n->foreign) {
        //foreign structs/unions have no body.
        out << "extern " << str_uni << n->getIdent() << ";\n";
        return;
    }
    out << str_uni << n->getIdent() << " {\n";
    for(auto c : (*(n->getChildren()))) {
        if(c->nodeType() == AstNodeType::VarDec) {
            auto vdec = static_cast<VarDeclNode*>(c);
            auto var = static_cast<VarNode*>(vdec->getLHS());
            std::string type_str = generateTypeString(vdec->mtypeinfo);
            out << type_str << ' ' << var->getVarName() << ";\n";
        }
    }
    out << "};\n";
}

void printFunctionHeader(FuncDefNode* fn, std::ofstream& out, bool is_proto) {
    auto typeinfo = fn->mtypeinfo;
    std::string return_type = generateTypeString(typeinfo);
    std::string extern_mod = "";
    std::string fn_name = "";
    if(fn->nodeType() == AstNodeType::Prototype) {
        extern_mod = "extern ";
        fn_name = fn->mfuncname;
    } else {
        fn_name = fn->mangledName();
    }
    out << extern_mod << return_type << " " << fn_name << '(';

    auto params = fn->getParameters();
    bool add_comma = false;
    for(auto p : params) {
        if(add_comma) out << ", ";
        std::string type_str = generateTypeString(p->mtypeinfo);
        std::string param_name = static_cast<ParamsNode*>(p)->mname;
        out << type_str << ' ' << param_name;
        add_comma = true;
    }
    if(is_proto) {
        out << ");\n";
    } else {
        out << ") ";
    }
}

void printBody(AstNode* n, std::ofstream& out) {
    switch(n->nodeType()) {
        case AstNodeType::Block:
            {
                out << "{\n";
                //print out statements
                for(auto c : (*(n->getChildren()))) {
                    printBody(c,out);
                    out << ";\n";
                }
                out << "}\n";
            }
            break;
        case AstNodeType::IfStmt:
            {
                auto ifn = static_cast<IfNode*>(n);
                out << "if(";
                printBody(ifn->getConditional(),out);
                out << ") ";
                printBody(ifn->getThen(),out);
                if(ifn->getThen()->nodeType() != AstNodeType::Block) {
                    out << ";\n";
                }
                auto elsen = ifn->getElse();
                if(elsen) {
                    out << "else ";
                    printBody(elsen,out);
                    if(elsen->nodeType() != AstNodeType::Block) {
                        out << ";\n";
                    }
                }
            }
            break;
        case AstNodeType::WhileLoop:
            {
                auto whilen = static_cast<LoopNode*>(n);
                out << "while(";
                printBody(whilen->getConditional(),out);
                out << ") ";
                printBody(whilen->getBody(),out);
                if(whilen->getBody()->nodeType() != AstNodeType::Block) {
                    out << ";\n";
                }
            }
            break;
        case AstNodeType::ForLoop:
            {
                auto forn = static_cast<LoopNode*>(n);
                out << "for(";
                printBody(forn->getInit(), out);
                out << "; ";
                printBody(forn->getConditional(), out);
                out << "; ";
                printBody(forn->getUpdate(), out);
                out << ") ";
                printBody(forn->getBody(),out);
                if(forn->getBody()->nodeType() != AstNodeType::Block) {
                    out << ";\n";
                }
            }
            break;
        case AstNodeType::LoopStmt:
            {
                auto node = static_cast<LoopStmtNode*>(n);
                if(node->isBreak) {
                    out << "break";
                } else {
                    out << "continue";
                }
            }
            break;
        case AstNodeType::Const:
            {
                auto node = static_cast<ConstantNode*>(n);
                if(node->mtypeinfo.type == SemanticType::Char && node->mtypeinfo.indirection) {
                    out << '"' << node->getVal() << '"';
                } else {
                    out << node->getVal();
                }
            }
            break;
        case AstNodeType::Cast:
            {
                auto node = static_cast<CastNode*>(n);
                auto end_t = node->mtypeinfo;
                std::string type_str = generateTypeString(end_t);
                out << '(' << type_str << ')';
                printBody(node->mchildren.at(0),out);
            }
            break;
        case AstNodeType::BinOp:
            {
                auto node = static_cast<BinOpNode*>(n);
                std::string op = node->getOp();
                if(node->opOverload != nullptr) {
                    auto overload = static_cast<FuncDefNode*>(node->opOverload);
                    out << overload->mangledName() << '(';
                    printBody(node->LHS(),out);
                    out << ',';
                    printBody(node->RHS(),out);
                    out << ')';
                } else {
                    if(node->unaryOp) {
                        if(op == "@") {
                            out << "(*(";
                            printBody(node->LHS(),out);
                            out << "))";
                        } else {
                            out << op;
                            printBody(node->LHS(),out);
                        }
                    } else {
                        if(op == ".") {
                            printBody(node->LHS(),out);
                            out << op;
                            printBody(node->RHS(),out);
                        } else {
                            out << '(';
                            printBody(node->LHS(),out);
                            out << ')';
                            out << op;
                            out << '(';
                            printBody(node->RHS(),out);
                            out << ')';
                        }
                    }
                }
            }
            break;
        case AstNodeType::FuncCall:
            {
                auto node = static_cast<FuncCallNode*>(n);
                std::string funcname;
                if(node->func == nullptr) {
                    funcname = node->mfuncname;
                } else {
                    funcname = node->func->mangledName();
                }
                out << funcname << '(';
                bool add_comma = false;
                for(auto c : node->mchildren) {
                    if(add_comma) out << ", ";
                    printBody(c,out);
                    add_comma = true;
                }
                out << ')';
            }
            break;
        case AstNodeType::RetStmnt:
            {
                if(n->mchildren.size() == 0) {
                    out << "return";
                } else {
                    out << "return ";
                    printBody(n->mchildren.at(0),out);
                }
            }
            break;
        case AstNodeType::Var:
            {
                auto node = static_cast<VarNode*>(n);
                out << node->getVarName();
            }
            break;
        case AstNodeType::VarDec:
            {
                auto node = static_cast<VarDeclNode*>(n);
                auto var = static_cast<VarNode*>(node->getLHS());
                std::string type_str = generateTypeString(var->mtypeinfo);
                out << type_str << ' ' << var->getVarName();
            }
            break;
        case AstNodeType::VarDecAssign:
            {
                auto node = static_cast<VarDeclNode*>(n);
                auto var = static_cast<VarNode*>(node->getLHS());
                std::string type_str;
                if(node->mtypeinfo.type != SemanticType::Typeless && node->mtypeinfo.type != SemanticType::Infer) {
                    type_str = generateTypeString(node->mtypeinfo);
                } else {
                    type_str = generateTypeString(var->mtypeinfo);
                }
                out << type_str << ' ' << var->getVarName() << " = ";
                printBody(node->getRHS(),out);
            }
            break;
        case AstNodeType::Assign:
            {
                auto node = static_cast<AssignNode*>(n);
                printBody(node->getLHS(),out);
                out << " = ";
                printBody(node->getRHS(),out);
            }
            break;
        default:
            std::cout << "DEFAULT CASE C-Codegen!!!\n";
            break;
    }
}

void cBackend(AstNode* ast, std::string filename) {
    std::vector<FuncDefNode*> funcs;
    getFunctionNodes(ast, funcs);
    std::vector<StructDefNode*> types;
    getUserTypeNodes(ast, types);

    //Print out the header
    std::string guard = filename;
    filename = filename.replace(filename.find(".nro"),4,"");
    guard = guard.replace(guard.find("."),1,"_");
    std::ofstream header(filename+"_neuro.h",std::ofstream::out);
    header << "#ifndef " << guard << '\n';
    header << "#define " << guard << '\n';
    header << "#include <stdint.h>\n";

    DEBUGC(std::cout << "fwd decls of types\n";)

    // Forward declare types
    header << "typedef uint8_t u8;\n";
    header << "typedef uint16_t u16;\n";
    header << "typedef uint32_t u32;\n";
    header << "typedef uint64_t u64;\n";
    header << "typedef int8_t s8;\n";
    header << "typedef int16_t s16;\n";
    header << "typedef int32_t s32;\n";
    header << "typedef int64_t s64;\n";
    header << "typedef float f32;\n";
    header << "typedef double f64;\n";
    for(auto n : types) {
        std::string struct_name = n->getIdent();
        const char* struct_or_union = (n->nodeType() == AstNodeType::StructDef) ? "typedef struct " : "typedef union ";
        header << struct_or_union << struct_name << ' ' << struct_name <<  ";\n";
    }

    DEBUGC(std::cout << "Simple types\n";)
    // Types
    std::set<StructDefNode*> printed_types;
    //Write out simple types first
    for(auto n : types) {
        bool is_simple = true;
        for(auto c : (*(n->getChildren()))) {
            if(c->nodeType() == AstNodeType::VarDec) {
                if((c->mtypeinfo.type == SemanticType::User) && (c->mtypeinfo.indirection == 0)) {
                    is_simple = false;
                    break;
                }
            }
        }
        if(is_simple) {
            //it was a simple struct
            printed_types.insert(n);
            printStruct(n,header);
        }
    }

    DEBUGC(std::cout << "Complex types\n";)
    //Print structs if all members have their type printed out already
    bool did_change;
    do {
        did_change = false;
        bool finished = true;
        for(auto n : types) {
            //skip structs we already printed
            if(printed_types.find(n) != printed_types.end()) {
                continue;
            }
            //we have a struct we haven't printed, we aren't finished
            finished = false;

            //can we print out every member?
            bool can_print = true;
            for(auto c : (*(n->getChildren()))) {
                if(c->nodeType() == AstNodeType::VarDec) {
                    if((c->mtypeinfo.type == SemanticType::User) && (c->mtypeinfo.indirection == 0)) {
                        auto type_str = c->mtypeinfo.userid;
                        bool struct_is_printed = false;
                        for(auto s : printed_types) {
                            //if(s->getIdent() == type_str) {
                            if(std::strcmp(s->getIdent().c_str(),type_str) == 0) {
                                struct_is_printed = true;
                                break;
                            }
                        }
                        if(!struct_is_printed) {
                            //can't print this struct has a member of a struct type not printed yet
                            can_print = false;
                            break;
                        }
                    }
                }
            }

            if(can_print) {
                did_change = true;
                printed_types.insert(n);
                printStruct(n,header);
                break;
            }
        }
        if(finished) {
            did_change = true;
            break;
        }
    } while(did_change);
    //NOTE(marcus): There should not be any circular dependencies if this code is run as this is
    //checked in an earlier pass.
    //TODO(marcus): use the earlier pass to construct the list of structs in a correct order instead
    //of redoing the work here.
    if(!did_change) std::cout << "Couldn't resolve circular dependency with types!\n";

    DEBUGC(std::cout << "Prototypes\n";)
    // Prototypes
    for(auto fn : funcs) {
        printFunctionHeader(fn, header, true);
    }

    header << "#endif" << '\n';


    //Print out the main c file
    std::ofstream cfile(filename+"_neuro.c",std::ofstream::out);
    cfile << "#include \"" << filename << "_neuro.h" << "\"\n";

    DEBUGC(std::cout << "Functions\n";)
    //generate functions
    for(auto fn : funcs) {
        if(fn->nodeType() == AstNodeType::Prototype) {
            continue;
        }
        printFunctionHeader(fn, cfile, false);
        printBody(fn->getFunctionBody(),cfile);
        cfile << '\n';
    }
}
