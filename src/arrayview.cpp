#include "arrayview.h"

ArrayView::ArrayView() {
}

ArrayView::ArrayView(AstNode** n, unsigned int s) {
    this->ptr = n;
    this->size = s;
}

AstNode** ArrayView::begin() {
    return this->ptr;
}

AstNode** ArrayView::end() {
    if(ptr) {
        return ptr+size;
    }
    return ptr;
}
