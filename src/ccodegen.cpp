#include "ccodegen.h"
#include <fstream>
#include <iostream>
#include <set>
#include <locale>

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
        std::string funcname;
        if(n->nodeType() == AstNodeType::Prototype) {
            funcname = static_cast<PrototypeNode*>(n)->mfuncname;
        } else {
            funcname = static_cast<FuncDefNode*>(n)->mfuncname;
        }
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
