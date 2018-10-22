#ifndef TYPEINFO_H
#define TYPEINFO_H

#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>

enum class SemanticType {
    Bool,
    Int,
    Void,
    Char,
    Double,
    Float,
    Typeless,
    Infer,
    User,
    u8,
    u16,
    u32,
    u64,
    s8,
    s16,
    s32,
    s64,
    intlit,
    floatlit,
    nulllit,
    Array,
    Template
};

std::ostream& operator<<(std::ostream& os, const SemanticType& obj);

struct TypeInfo {
    const char* userid = nullptr;
    TypeInfo* base_t = nullptr;
    SemanticType type = SemanticType::Typeless;
    int pindirection = 0;
    int arr_size = 0;
    std::unordered_map<std::string,TypeInfo>* userTypeParameterTypes = nullptr;
    int indirection() const;
    bool isPointer() const;
    bool isArray() const;
    int arraySize() const;
};

bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);

std::ostream& operator<<(std::ostream& os, const TypeInfo& obj);

std::string TypeInfoToString(const TypeInfo& ti);
int decreaseDerefTypeInfo(TypeInfo& t);
void increaseDerefTypeInfo(TypeInfo& t);
#endif
