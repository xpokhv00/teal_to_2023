/**
 * @file expression.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "common.h"
#include "scanner.h"
#include "generator.h"
#include "types.h"
#include "symtable.h"
#include <stdlib.h>
#include <stdbool.h>

#define ALLOCATION_CHUNK 256
#define MAX_RULE_LENGTH 10

typedef enum {
    S_NONE = 0,
    S_EXPR,
    S_VALUE,
    S_EMPTY,
    S_MULDIV,
    S_ADDSUB,
    S_COMPARE,
    S_EQNEQ,
    S_PARL,
    S_PARR,
    S_HANDLE,
    S_GETLEN,
    NUM_SYMBOLS,
} SymbolType;

typedef struct {
    SymbolType symbolType;
    Type varType;
    Token token;
} Symbol;

typedef struct {
    int allocatedElements;
    int top;
    Symbol *data;
} SymStack;

typedef struct {
    SymbolType from;
    SymbolType to[MAX_RULE_LENGTH];
    void (*fn)(SymStack *s);
} Rule;

void symstack_init(SymStack* s);
Status symstack_push(SymStack* s, Symbol item);
Symbol symstack_top(SymStack* s);
Symbol symstack_pop(SymStack* s);
bool symstack_is_empty(SymStack* s);
int symstack_count(SymStack* s);
void symstack_destroy(SymStack* s);

// The only function, that is called from parser
// It parses expressions, check for correct semantics and generates code
bool nt_expr(Token *pToken, SymTab *symTab, Status *status, Type *returnType);





#endif // EXPRESSION_H
