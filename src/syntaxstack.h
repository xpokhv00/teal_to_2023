/**
 * @file syntaxstack.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#ifndef SYNTAXSTACK_H
#define SYNTAXSTACK_H

#include "common.h"
#include "scanner.h"

#include <stdlib.h>
#include <stdbool.h>

#define ALLOCATION_CHUNK 256

typedef struct {
    int allocatedElements;
    int top;
    TokenType *data;
} SyntaxStack;

void syntaxstack_init(SyntaxStack* s);

Status syntaxstack_push(SyntaxStack* s, TokenType item);

TokenType syntaxstack_top(SyntaxStack* s);

TokenType syntaxstack_pop(SyntaxStack* s);

bool syntaxstack_is_empty(SyntaxStack* s);

void syntaxstack_destroy(SyntaxStack* s);



#endif // SYNTAXSTACK_H
