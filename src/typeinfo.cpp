#include "typeinfo.h"

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

bool operator==(const TypeInfo& lhs, const TypeInfo& rhs) {
    if(lhs.type != rhs.type) return false;
    if(lhs.pindirection != rhs.pindirection) return false;
    if(lhs.arr_size != rhs.arr_size) return false;

    if(lhs.type == SemanticType::User && lhs.userid && rhs.userid) {
        return strcmp(lhs.userid,rhs.userid) == 0;
    } else {
        return true;
    }
    //TODO(marcus): compare template types?
    //TODO(marcus): compare array types?
}

bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs) {
    return !(lhs == rhs);
}

std::string TypeInfoToString(const TypeInfo& ti) {
    std::string mangled;

    //TODO(marcus): will need to represent arrays
    const int indirection = ti.indirection();
    if(indirection) {
        mangled = "p" + std::to_string(indirection);
    }
    switch(ti.type) {
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
            mangled += ti.userid;
            break;
        default:
            mangled += "_default_";
            break;
    }
    return mangled;
}

int decreaseDerefTypeInfo(TypeInfo& t) {
    if(t.indirection() > 0) {
        t.pindirection -= 1;
    } else {
        return 1;
    }
    return 0;
}

void increaseDerefTypeInfo(TypeInfo& t) {
    t.pindirection += 1;
    return;
}
