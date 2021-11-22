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

// This function returns < or > or e or =
// based on token which is stored on top of the stack and based on token which it recieves on begining
// ********************************** BIG NOTE ***************************************************
// THIS FUNCTION DOES NOT TAKE INTO ACCOUNT END OF THE EXPRESSION
char check_table(Token *pToken, SymStack s)
{
    // deciding for empty stack
    if (symstack_is_empty(&s))
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '<';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '<';
        if (pToken->type == TOKEN_PAR_L) return '<';
        if (pToken->type == TOKEN_PAR_R) return 'e';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return '<';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '<';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '>';
    }
    // deciding for + -
    if ( symstack_top(&s) == (SymbolType)TOKEN_PLUS || symstack_top(&s) == (SymbolType)TOKEN_MINUS)
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '>';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '<';
        if (pToken->type == TOKEN_PAR_L) return '<';
        if (pToken->type == TOKEN_PAR_R) return '>';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return '<';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '>';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '>';
    }

    // deciding for */
    if ( symstack_top(&s) == (SymbolType)TOKEN_MULTIPLY || symstack_top(&s) == (SymbolType)TOKEN_DIVIDE || symstack_top(&s) == (SymbolType)TOKEN_INT_DIVIDE)
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '>';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '>';
        if (pToken->type == TOKEN_PAR_L) return '<';
        if (pToken->type == TOKEN_PAR_R) return '>';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return '<';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '>';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '>';
    }

    // deciding for left par
    if ( symstack_top(&s) == (SymbolType)TOKEN_PAR_L )
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '<';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '<';
        if (pToken->type == TOKEN_PAR_L) return '<';
        if (pToken->type == TOKEN_PAR_R) return '=';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return '<';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '<';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '<';
    }

    // deciding for right par
    if ( symstack_top(&s) == (SymbolType)TOKEN_PAR_R)
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '>';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '>';
        if (pToken->type == TOKEN_PAR_L) return 'e';
        if (pToken->type == TOKEN_PAR_R) return '>';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return 'e';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '>';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '>';
    }

    // deciding for number
    if ( symstack_top(&s) == (SymbolType)TOKEN_INTEGER_LIT || symstack_top(&s) == (SymbolType)TOKEN_DOUBLE_LIT)
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '>';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '>';
        if (pToken->type == TOKEN_PAR_L) return 'e';
        if (pToken->type == TOKEN_PAR_R) return '>';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return 'e';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '>';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '>';
    }

    // deciding for <= >= < >
    if ( symstack_top(&s) == (SymbolType)TOKEN_LT || symstack_top(&s) == (SymbolType)TOKEN_GT || symstack_top(&s) == (SymbolType)TOKEN_GEQ || symstack_top(&s) == (SymbolType)TOKEN_LEQ)
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '<';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '<';
        if (pToken->type == TOKEN_PAR_L) return '<';
        if (pToken->type == TOKEN_PAR_R) return '>';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return '<';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '>';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '>';
    }

    //deciding for == !=
    if ( symstack_top(&s) == (SymbolType)TOKEN_EQUALS || symstack_top(&s) == (SymbolType)TOKEN_NEQ)
    {
        if (pToken->type == TOKEN_PLUS || pToken->type == TOKEN_MINUS) return '>';
        if (pToken->type == TOKEN_DIVIDE || pToken->type == TOKEN_INT_DIVIDE || pToken->type == TOKEN_MULTIPLY) return '<';
        if (pToken->type == TOKEN_PAR_L) return '<';
        if (pToken->type == TOKEN_PAR_R) return '>';
        if (pToken->type == TOKEN_INTEGER_LIT || pToken->type == TOKEN_DOUBLE_LIT) return '<';
        if (pToken->type == TOKEN_LT || pToken->type == TOKEN_GT || pToken->type == TOKEN_GEQ || pToken->type == TOKEN_LEQ) return '>';
        if (pToken->type == TOKEN_EQUALS || pToken->type == TOKEN_NEQ) return '>';
    }
    return 'e';
}


bool nt_expr(Token *pToken)
{

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

    TokenType tmp;

    // function uses token "NONE" as indetier of stack bottom
    // function uses token "TOKEN_NIL" as indetier of handel
    // function uses token "TOKEN_END" as identifier of correct expression
    symstack_push(&s, (SymbolType) NONE);
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
            switch (check_table(pToken, s))
            {
                case '=':
                    symstack_push(&s, (SymbolType) pToken->type);
                    status = scanner_get_token(pToken);
                    continue;
                case '<':
                    tmp = symstack_pop(&s);
                    symstack_push(&s, (SymbolType)TOKEN_NIL);
                    symstack_push(&s, (SymbolType)tmp);
                    symstack_push(&s, (SymbolType)pToken->type);
                    status = scanner_get_token(pToken);
                    continue;
                case '>':
                    for (int i = s->top; i > 0; i --)
                    {
                        if (s->data[i] == TOKEN_NIL)
                        {
                            // TO DO
                        }
                    }
                default :
                    return false;
            }
        }
        else
        {
            break;
        }
    }

    // cyklus bude podobny ako predosly, len v tomto cykle budeme vediet ze sa jedna o stav kedy  prisiel na vstup koniec vyrazu
    //while (symstack_top(&s))




    symstack_destroy(&s);
    scanner_destroy_token(pToken);
    status = scanner_get_token(pToken);
    if (status != SUCCESS)
    {
        return false;
    }
    return true;
}