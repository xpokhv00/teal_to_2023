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




void symstack_init(SymStack* s)
{
    s->data = NULL;
    s->top = -1;
    s->allocatedElements = 0;
}

Status symstack_push(SymStack* s, SymbolType item)
{
    // reallocate if not enough space
    if ((s->top + 1) >= s->allocatedElements)
    {
        int numElements = s->allocatedElements + ALLOCATION_CHUNK;

        SymbolType *newPtr = realloc(s->data, numElements * sizeof(TokenType));
        if (newPtr == NULL)
        {
            return ERR_INTERNAL;
        }
        s->allocatedElements = numElements;
        s->data = newPtr;
    }

    s->top++;
    s->data[s->top] = item;
    return SUCCESS;
}

SymbolType symstack_top(SymStack* s)
{
    if (!symstack_is_empty(s))
    {
        return s->data[s->top];
    }
    return (SymbolType)NONE;
}

SymbolType symstack_pop(SymStack* s)
{
    if (!symstack_is_empty(s))
    {
        return s->data[s->top--];
    }
    return (SymbolType)NONE;
}

bool symstack_is_empty(SymStack* s)
{
    return s->top == -1;
}

void symstack_destroy(SymStack* s)
{
    free(s->data);
    s->data = NULL;
    s->top = -1;
    s->allocatedElements = 0;
}


bool nt_expr(Token *pToken)
{
    // TODO this is where you begin

    Status status;

    SymStack s;
    symstack_init(&s);

    int num_of_val_tokens = 16;
    TokenType valid_tokens[num_of_val_tokens];

    valid_tokens[0]  = TOKEN_PLUS;
    valid_tokens[1]  = TOKEN_MINUS;
    valid_tokens[2]  = TOKEN_DIVIDE;
    valid_tokens[3]  = TOKEN_INT_DIVIDE;
    valid_tokens[4]  = TOKEN_MULTIPLY;
    valid_tokens[5]  = TOKEN_EQUALS;
    valid_tokens[6]  = TOKEN_LT;
    valid_tokens[7]  = TOKEN_LEQ;
    valid_tokens[8]  = TOKEN_GT;
    valid_tokens[9]  = TOKEN_GEQ;
    valid_tokens[10] = TOKEN_NEQ;
    valid_tokens[11] = TOKEN_PAR_L;
    valid_tokens[12] = TOKEN_PAR_R;
    valid_tokens[13] = TOKEN_GET_LENGTH;
    valid_tokens[14] = TOKEN_DOUBLE_LIT;
    valid_tokens[15] = TOKEN_INTEGER_LIT;



    bool compatible = true;
    while (true)
    {
        // this cycle checks if pToken == +, - , / , * , = , < , <= , > , >= , ~= , ( , ) , int , double
        for (int i = 0; i < 16; i++)
        {
            if (pToken->type == valid_tokens[i]) break;
            if (i == num_of_val_tokens - 1) compatible = false;
        }

        if (compatible)
        {
            symstack_push(&s, (SymbolType) pToken->type);
        }
        else
        {
            break;
        }
        status = scanner_get_token(pToken);
    }

    symstack_destroy(&s);
    scanner_destroy_token(pToken);
    status = scanner_get_token(pToken);
    if (status != SUCCESS)
    {
        return false;
    }
    return true;
}