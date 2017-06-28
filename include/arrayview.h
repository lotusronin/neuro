#ifndef ARRAYVIEW_H
#define ARRAYVIEW_H

#include "astnode.h"

class ArrayView {
    public:
        AstNode** ptr = nullptr;
        unsigned int size = 0;
        ArrayView();
        ArrayView(AstNode** n, unsigned int s);
        AstNode** begin();
        AstNode** end();
};

#endif //ARRAYVIEW_H
