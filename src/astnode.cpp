#include "astnode.h"
#include <iostream>
#include <cassert>
#include <cstring>

AstNode::AstNode() {
}

AstNode::~AstNode() {
}

AstNodeType AstNode::nodeType() const {
    return mnodet;
}

SemanticType AstNode::getType() const {
    return mtypeinfo.type;
}

void AstNode::addChild(AstNode* child) {
    //std::cout << "addChild not called on subclass\n";
    mchildren.push_back(child);
}

AstNode* AstNode::lastChild() {
    //std::cout << "lastChild not called on subclass\n";
    return mchildren.back();
}

std::vector<AstNode*>* AstNode::getChildren() {
    //std::cout << "getChildren not called on subclass\n";
    return &mchildren;
}

void AstNode::setToken(Token& t) {
    mtoken = t;
}

TypeInfo* AstNode::getTypeInfo() {
    return &mtypeinfo;
}

std::ostream& operator<<(std::ostream& os, const SemanticType& obj) {
    const char* out;
    switch(obj) {
        case SemanticType::Bool:
            out = "bool";
            break;
        case SemanticType::Int:
            out = "int";
            break;
        case SemanticType::Void:
            out = "void";
            break;
        case SemanticType::Char:
            out = "char";
            break;
        case SemanticType::Double:
            out = "double";
            break;
        case SemanticType::Float:
            out = "float";
            break;
        case SemanticType::Typeless:
            out = "typeless";
            break;
        case SemanticType::Infer:
            out = "Inferred";
            break;
        case SemanticType::intlit:
            out = "intlit";
            break;
        case SemanticType::User:
            out = "";
            break;
        case SemanticType::u32:
            out = "u32";
            break;
        default:
            out = "Unrecognized value!";
            break;
    }
    os << out;
    return os;
}

std::ostream& operator<<(std::ostream& os, const TypeInfo& obj) {
    if(obj.indirection() > 0) {
        int i = obj.indirection();
        while(i) {
            os << '*';
            --i;
        }
    }
    os << obj.type;
    if(obj.userid) {
        os << obj.userid;
    }
    return os;
}

int TypeInfo::indirection() const {
    return pindirection;
}

bool TypeInfo::isPointer() const {
    return (pindirection > 0);
}

bool TypeInfo::isArray() const {
    return (arr_size > 0);
}

int TypeInfo::arraySize() const {
    return arr_size;
}



int AssignNode::count = 0;

AssignNode::AssignNode() {
    id = AssignNode::count;
    AssignNode::count++;
    mnodet = AstNodeType::Assign;
}

AssignNode::AssignNode(AssignNode* n) {
    id = AssignNode::count;
    AssignNode::count++;
    mnodet = AstNodeType::Assign;
    mtoken = n->mtoken;
}

AssignNode::~AssignNode() {
}

void AssignNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

AstNode* AssignNode::getLHS() {
    return mchildren.at(0);
}

AstNode* AssignNode::getRHS() {
    return mchildren.at(1);
}



int BinOpNode::count = 0;
int BinOpNode::deleted = 0;
int BinOpNode::constructed = 0;

BinOpNode::BinOpNode(AstNodeType ntype) {
    id = BinOpNode::count;
    BinOpNode::count++;
    BinOpNode::constructed++;
    unaryOp = false;
    mchildren.reserve(2);
    opOverload = nullptr;
    mnodet = ntype;
}

BinOpNode::BinOpNode(BinOpNode* n) {
    id = BinOpNode::count;
    BinOpNode::count++;
    BinOpNode::constructed++;
    unaryOp = n->unaryOp;
    mchildren.reserve(2);
    opOverload = n->opOverload;
    mnodet = AstNodeType::BinOp;
    mtoken = n->mtoken;
    mop = n->mop;
    mnodet = n->mnodet;
}

BinOpNode::~BinOpNode() {
    BinOpNode::deleted += 1;
}

void BinOpNode::setOp(const char* op) {
    mop = op;
}

const char* BinOpNode::getOp() const {
    return mop;
}

AstNode* BinOpNode::LHS() {
    if(mchildren.size() > 0) {
        return mchildren[0];
    }
    return nullptr;
}

AstNode* BinOpNode::RHS() {
    if(mchildren.size() > 1) {
        return mchildren[1];
    }
    return nullptr;
}

void BinOpNode::setLHS(AstNode* ast) {
        mchildren[0] = ast;
}

void BinOpNode::setRHS(AstNode* ast) {
        mchildren[1] = ast;
}

void BinOpNode::setToken(Token& t) {
    mtoken = t;
    setOp(t.token);
}

SemanticType BinOpNode::getType() const {
    return mtypeinfo.type;
}

void BinOpNode::printDeleted() {
    std::cout << "Constructed binop nodes " << BinOpNode::constructed << '\n';
    std::cout << "Deleted binop nodes: " << BinOpNode::deleted << '\n';
}



int BlockNode::count = 0;

BlockNode::BlockNode() {
    id = BlockNode::count;
    BlockNode::count++;
    mnodet = AstNodeType::Block;
}

BlockNode::~BlockNode() {
}

int BlockNode::getId() const {
    return id;
}



int CastNode::count = 0;

CastNode::CastNode() {
    id = CastNode::count;
    CastNode::count++;
    mnodet = AstNodeType::Cast;
}

CastNode::CastNode(CastNode* n) {
    id = CastNode::count;
    CastNode::count++;
    mnodet = AstNodeType::Cast;
    mtoken = n->mtoken;
    mtypeinfo = n->mtypeinfo;
    fromType = n->fromType;
}

CastNode::~CastNode() {
}

void CastNode::addChild(AstNode* n) {
    //TODO(marcus): should we allow more than 1 child for Cast nodes?
    mchildren.push_back(n);
}



int CompileUnitNode::count = 0;

CompileUnitNode::CompileUnitNode() {
    id = CompileUnitNode::count;
    CompileUnitNode::count++;
    mnodet = AstNodeType::CompileUnit;
}

CompileUnitNode::~CompileUnitNode() {
}

void CompileUnitNode::setFileName(std::string name) {
    mname = name;
}

std::string CompileUnitNode::getFileName() const {
    return mname;
}



int ConstantNode::count = 0;

ConstantNode::ConstantNode() {
    id = ConstantNode::count;
    ConstantNode::count++;
    mnodet = AstNodeType::Const;
}
ConstantNode::ConstantNode(ConstantNode* n) {
    id = ConstantNode::count;
    ConstantNode::count++;
    mnodet = AstNodeType::Const;
    mtoken = n->mtoken;
    mval = n->mval;
    mtypeinfo = n->mtypeinfo;
}

ConstantNode::~ConstantNode() {
}

void ConstantNode::addChild(AstNode* node) {
    std::cerr << "Should not be adding child to constant!\n";
}

std::vector<AstNode*>* ConstantNode::getChildren() {
    return &mchildren;
}

void ConstantNode::setVal(const char* val) {
    mval = val;
    if(mtypeinfo.type == SemanticType::nulllit) {
        mval = "0";
    }
}

const char* ConstantNode::getVal() const {
    return mval;
}

void ConstantNode::setToken(const Token& t) {
    mtoken = t;
    SemanticType mstype = SemanticType::Typeless;
    if(t.type == TokenType::intlit) {
        mstype = SemanticType::intlit;
    } else if(t.type == TokenType::floatlit) {
        mstype = SemanticType::floatlit;
    } else if(t.type == TokenType::strlit) {
        //TODO(marcus): maybe fix this up?
        //TODO(marcus): this might break if someone else tries to set mtypeinfo later
        mstype = SemanticType::Char;
        mtypeinfo.pindirection = 1;
    } else if(t.type == TokenType::charlit) {
        mstype = SemanticType::Char;
    } else if(t.type == TokenType::vnull) {
        mstype = SemanticType::nulllit;
    }
    mtypeinfo.type = mstype;
}



int DeferStmtNode::count = 0;

DeferStmtNode::DeferStmtNode() {
    id = DeferStmtNode::count;
    DeferStmtNode::count++;
    mnodet = AstNodeType::DeferStmt;
}

DeferStmtNode::~DeferStmtNode() {
}



int FuncCallNode::count = 0;

FuncCallNode::FuncCallNode() {
    id = FuncCallNode::count;
    FuncCallNode::count++;
    mnodet = AstNodeType::FuncCall;
    func = nullptr;
}

FuncCallNode::FuncCallNode(FuncCallNode* n) {
    id = FuncCallNode::count;
    FuncCallNode::count++;
    mnodet = AstNodeType::FuncCall;
    func = n->func;
    mfuncname = n->mfuncname;
    scopes = n->scopes;
}

FuncCallNode::~FuncCallNode() {
}

void FuncCallNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}



int FuncDefNode::count = 0;

FuncDefNode::FuncDefNode(AstNodeType ntype) {
    id = FuncDefNode::count;
    FuncDefNode::count++;
    mchildren.reserve(8);
    isOperatorOverload = 0;
    op = nullptr;
    mnodet = ntype;
    isTemplated = 0;
}

FuncDefNode::FuncDefNode(FuncDefNode* n) {
    id = FuncDefNode::count;
    FuncDefNode::count++;
    mchildren.reserve(n->mchildren.size());
    isOperatorOverload = n->isOperatorOverload;
    op = n->op;
    mnodet = n->mnodet;
    mfuncname = n->mfuncname;
    mtypeinfo = n->mtypeinfo;
    isTemplated = 0;
    mtoken = n->mtoken;
}

FuncDefNode::~FuncDefNode() {
}

void FuncDefNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

ArrayView FuncDefNode::getParameters() {
    auto s = mchildren.size();
    if(mnodet == AstNodeType::FuncDef) s -= 1;
    auto p = s ? &mchildren[0] : nullptr;
    ArrayView params(p,s);
    return params;
}

AstNode* FuncDefNode::getFunctionBody() {
    return mchildren.back();
}

std::string FuncDefNode::mangledName() {
    if(mfuncname == "main") {
        return mfuncname;
    }

    std::string mangled = mfuncname + "_";
    for(auto arg : mchildren) {
        if(arg->nodeType() != AstNodeType::Params)
            continue;
        auto argti = arg->mtypeinfo;
        auto indirection = argti.indirection();
        if(indirection) {
            mangled = mangled + "p" + std::to_string(indirection);
        }
        switch(argti.type) {
            case SemanticType::Void:
                mangled += "v";
                break;
            case SemanticType::Bool:
                mangled += "b";
                break;
            case SemanticType::Char:
                mangled += "c";
                break;
            case SemanticType::Int:
                mangled += "i";
                break;
            case SemanticType::Float:
                mangled += "f";
                break;
            case SemanticType::Double:
                mangled += "d";
                break;
            case SemanticType::u8:
                mangled += "u8";
                break;
            case SemanticType::u32:
                mangled += "u32";
                break;
            case SemanticType::u64:
                mangled += "u64";
                break;
            case SemanticType::s8:
                mangled += "s8";
                break;
            case SemanticType::s32:
                mangled += "s32";
                break;
            case SemanticType::s64:
                mangled += "s64";
                break;
            case SemanticType::User:
                mangled += argti.userid;
                break;
            default:
                mangled += "_default_";
                break;
        }
    }
    //std::cout << "Mangled: " << mangled << '\n';
    return mangled;
}



int IfNode::count = 0;

IfNode::IfNode() {
    id = IfNode::count;
    IfNode::count++;
    mnodet = AstNodeType::IfStmt;
}

IfNode::IfNode(IfNode* n) {
    id = IfNode::count;
    IfNode::count++;
    mnodet = AstNodeType::IfStmt;
    mtoken = n->mtoken;
}

IfNode::~IfNode() {
}

AstNode* IfNode::getConditional() {
    return mchildren.at(0);
}

AstNode* IfNode::getThen() {
    return mchildren.at(1);
}

AstNode* IfNode::getElse() {
    bool hasElse = mchildren.size() == 3;
    return hasElse ? mchildren[2] : nullptr;
}



int LoopNode::count = 0;

LoopNode::LoopNode(AstNodeType ntype) {
    id = LoopNode::count;
    LoopNode::count++;
    mnodet = ntype;
}

LoopNode::LoopNode(LoopNode* n) {
    id = LoopNode::count;
    LoopNode::count++;
    mnodet = n->mnodet;
    mtoken = n->mtoken;
}

LoopNode::~LoopNode() {
}

AstNode* LoopNode::getInit() {
    assert(mnodet == AstNodeType::ForLoop);
    return mchildren.at(0);
}

AstNode* LoopNode::getConditional() {
    //for loop at 1
    //while loop at 0
    int idx = 0;
    if(mnodet == AstNodeType::ForLoop) idx = 1;
    return mchildren.at(idx);
}

AstNode* LoopNode::getUpdate() {
    assert(mnodet == AstNodeType::ForLoop);
    return mchildren.at(2);
}

AstNode* LoopNode::getBody() {
    //for loop at 3
    //while loop at 1
    int idx = 1;
    if(mnodet == AstNodeType::ForLoop) idx = 3;
    return mchildren.at(idx);
}

int LoopNode::getId() const {
    return id;
}



int LoopStmtNode::count = 0;

LoopStmtNode::LoopStmtNode() {
    id = LoopStmtNode::count;
    LoopStmtNode::count++;
    mnodet = AstNodeType::LoopStmtBrk;
}

LoopStmtNode::LoopStmtNode(LoopStmtNode* n) {
    id = LoopStmtNode::count;
    LoopStmtNode::count++;
    mnodet = n->mnodet;
    mtoken = n->mtoken;
}

LoopStmtNode::~LoopStmtNode() {
}



int ParamsNode::count = 0;

ParamsNode::ParamsNode() {
    id = ParamsNode::count;
    ParamsNode::count++;
    mnodet = AstNodeType::Params;
}

ParamsNode::ParamsNode(ParamsNode* n) {
    id = ParamsNode::count;
    ParamsNode::count++;
    mnodet = AstNodeType::Params;
    mtoken = n->mtoken;
    mtypeinfo = n->mtypeinfo;
    mname = n->mname;
}

ParamsNode::~ParamsNode() {
}

void ParamsNode::addParamName(std::string& name) {
    mname = name;
}



ProgramNode::ProgramNode() {
    mnodet = AstNodeType::Program;
}

ProgramNode::~ProgramNode() {
}



int ReturnNode::count = 0;

ReturnNode::ReturnNode() {
    id = ReturnNode::count;
    ReturnNode::count++;
    mchildren.reserve(1);
    mnodet = AstNodeType::RetStmnt;
}

ReturnNode::ReturnNode(ReturnNode* n) {
    id = ReturnNode::count;
    ReturnNode::count++;
    mchildren.reserve(1);
    mnodet = AstNodeType::RetStmnt;
    mtoken = n->mtoken;
}

ReturnNode::~ReturnNode() {
}



int SizeOfNode::count = 0;

SizeOfNode::SizeOfNode() {
    id = SizeOfNode::count;
    SizeOfNode::count++;
    mnodet = AstNodeType::SizeOf;
}

SizeOfNode::~SizeOfNode() {
}



int StructDefNode::count = 0;

StructDefNode::StructDefNode(AstNodeType nodet) {
    id = StructDefNode::count;
    StructDefNode::count++;
    foreign = false;
    mnodet = nodet;
}

StructDefNode::~StructDefNode() {
}

std::string& StructDefNode::getIdent() {
    return ident;
}



int VarDeclNode::count = 0;

VarDeclNode::VarDeclNode(AstNodeType ntype) {
    id = VarDeclNode::count;
    VarDeclNode::count++;
    mchildren.reserve(2);
    mnodet = ntype;
}

VarDeclNode::VarDeclNode(VarDeclNode* n) {
    id = VarDeclNode::count;
    VarDeclNode::count++;
    mchildren.reserve(2);
    mnodet = n->mnodet;
    mtypeinfo = n->mtypeinfo;
    mtoken = n->mtoken;
}

VarDeclNode::~VarDeclNode() {
}

AstNode* VarDeclNode::getLHS() {
    return mchildren.at(0);
}

AstNode* VarDeclNode::getRHS() {
    assert(mnodet == AstNodeType::VarDecAssign);
    return mchildren.at(1);
}



int VarNode::count = 0;

VarNode::VarNode() {
    id = VarNode::count;
    VarNode::count++;
    mnodet = AstNodeType::Var;
}

VarNode::VarNode(VarNode* n) {
    id = VarNode::count;
    VarNode::count++;
    mnodet = AstNodeType::Var;
    mname = n->mname;
    mtypeinfo = n->mtypeinfo;
}

VarNode::~VarNode() {
}

void VarNode::addVarName(std::string name) {
    mname = name;
}

const char* VarNode::getVarName() const {
    return mname.c_str();
}
