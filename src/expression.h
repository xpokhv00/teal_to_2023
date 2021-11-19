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
#include <stdlib.h>
#include <stdbool.h>

#define ALLOCATION_CHUNK 256

// extends TokenType
typedef enum {
    NT_EXPR = NUM_TOKENS,
    NT_EXPR_ID,
    NT_EXPR_OP,
} SymbolType;

typedef struct {
    int allocatedElements;
    int top;
    SymbolType *data;
} SymStack;

void symstack_init(SymStack* s);
Status symstack_push(SymStack* s, SymbolType item);
SymbolType symstack_top(SymStack* s);
SymbolType symstack_pop(SymStack* s);
bool symstack_is_empty(SymStack* s);
void symstack_destroy(SymStack* s);

// The only function, that is called from parser
// It parses expressions, check for correct semantics and generates code
bool nt_expr(Token *pToken);





#endif // EXPRESSION_H
