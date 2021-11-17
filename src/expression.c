/**
 * @file expression.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "expression.h"




void symstack_init(SymStack* s) {
    s->data = NULL;
    s->top = -1;
    s->allocatedElements = 0;
}

Status symstack_push(SymStack* s, SymbolType item) {
    // reallocate if not enough space
    if ((s->top + 1) >= s->allocatedElements) {
        int numElements = s->allocatedElements + ALLOCATION_CHUNK;

        SymbolType *newPtr = realloc(s->data, numElements * sizeof(TokenType));
        if (newPtr == NULL) {
            return ERR_INTERNAL;
        }
        s->allocatedElements = numElements;
        s->data = newPtr;
    }

    s->top++;
    s->data[s->top] = item;
    return SUCCESS;
}

SymbolType symstack_top(SymStack* s) {
    if (!symstack_is_empty(s)) {
        return s->data[s->top];
    }
    return (SymbolType)NONE;
}

SymbolType symstack_pop(SymStack* s) {
    if (!symstack_is_empty(s)) {
        return s->data[s->top--];
    }
    return (SymbolType)NONE;
}

bool symstack_is_empty(SymStack* s) {
    return s->top == -1;
}

void symstack_destroy(SymStack* s) {
    free(s->data);
    s->data = NULL;
    s->top = -1;
    s->allocatedElements = 0;
}

bool nt_expr() {
    // TODO this is where you begin
    SymStack s;
    symstack_init(&s);
    symstack_push(&s, (SymbolType)TOKEN_IDENTIFIER);
    symstack_pop(&s);
    symstack_destroy(&s);
    return true;
}