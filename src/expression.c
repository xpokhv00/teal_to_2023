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

const Symbol EMPTY_SYMBOL = { .symbolType = S_EMPTY };
const Symbol HANDLE = { .symbolType = S_HANDLE };
const Symbol GENERIC_EXPR = { .symbolType = S_EXPR, .varType=INTEGER };
const Symbol NONE_SYMBOL = { .symbolType = S_NONE };

// for all the functions to use
static SymTab *st;

void symstack_init(SymStack* s)
{
    s->data = NULL;
    s->top = -1;
    s->allocatedElements = 0;
}

Status symstack_push(SymStack* s, Symbol item)
{
    // reallocate if not enough space
    if ((s->top + 1) >= s->allocatedElements)
    {
        int numElements = s->allocatedElements + ALLOCATION_CHUNK;

        Symbol *newPtr = realloc(s->data, numElements * sizeof(Symbol));
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

Symbol symstack_top(SymStack* s) {
    if (!symstack_is_empty(s))
    {
        return s->data[s->top];
    }
    return NONE_SYMBOL;
}

// A function more powerful than top
// it can read n-th element from the top of the stack
// with depth=0 it behaves exactly like stack_top
Symbol symstack_peek(SymStack* s, int depth) {
    if (depth > s->top) {
        return NONE_SYMBOL;
    }
    return s->data[s->top-depth];
}

Symbol symstack_pop(SymStack* s)
{
    if (!symstack_is_empty(s))
    {
        return s->data[s->top--];
    }
    return NONE_SYMBOL;
}

bool symstack_is_empty(SymStack* s)
{
    return s->top <= -1;
}

int symstack_count(SymStack* s){
    return s->top + 1;
}

void symstack_destroy(SymStack* s)
{
    free(s->data);
    s->data = NULL;
    s->top = -1;
    s->allocatedElements = 0;
}

Symbol symstack_top_terminal(SymStack* s) {
    int i=0;
    while (i>=0) {
        // get the n-th symbol from the top
        Symbol current = s->data[s->top-i];
        // if it is a terminal, return it
        if (current.symbolType != S_EXPR) {
            return current;
        }
        i++;
    }
    return NONE_SYMBOL;
}

Status symstack_push_above_top_terminal(SymStack* s, Symbol item) {
    // reallocate if not enough space
    if ((s->top + 1) >= s->allocatedElements) {
        int numElements = s->allocatedElements + ALLOCATION_CHUNK;

        Symbol *newPtr = realloc(s->data, numElements * sizeof(Symbol));
        if (newPtr == NULL)
        {
            return ERR_INTERNAL;
        }
        s->allocatedElements = numElements;
        s->data = newPtr;
    }

    for (int i = s->top; i>=0; i--) {
        // while there are non-terminals
        if (s->data[i].symbolType == S_EXPR) {
            // shift them to the back
            s->data[i+1] = s->data[i];
        } else {
            // after you reach the first terminal, insert the item
            s->data[i+1] = item;
            break;
        }
    }

    s->top++;
    return SUCCESS;
}

SymbolType tokentype_to_symboltype(TokenType tt) {
    switch(tt) {
        case TOKEN_INTEGER_LIT:
        case TOKEN_DOUBLE_LIT:
        case TOKEN_STRING_LIT:
        case TOKEN_NIL:
        case TOKEN_IDENTIFIER:
            return S_VALUE;

        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
            return S_MULDIV;

        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return S_ADDSUB;

        case TOKEN_GT:
        case TOKEN_LT:
        case TOKEN_LEQ:
        case TOKEN_GEQ:
            return S_COMPARE;

        case TOKEN_EQUALS:
        case TOKEN_NEQ:
            return S_EQNEQ;

        case TOKEN_PAR_L:
            return S_PARL;

        case TOKEN_PAR_R:
            return S_PARR;

        case TOKEN_GET_LENGTH:
            return S_GETLEN;

        case TOKEN_CONCATENATE:
            return S_CONCAT;

        default:
            return S_NONE;
    }
}

Symbol token_to_symbol(Token token, SymTab *symTab) {
    Symbol symbol;
    symbol.varType = st_token_to_type(symTab, token),
    symbol.symbolType = tokentype_to_symboltype(token.type),
    symbol.token = token;
    return symbol;
}


char table_lookup(Symbol stackTop, Symbol inputSymbol) {
    SymbolType top = stackTop.symbolType;
    SymbolType in = inputSymbol.symbolType;

    switch (top) {
        case S_ADDSUB:
            switch (in) {
                case S_ADDSUB:
                case S_PARR:
                case S_COMPARE:
                case S_EQNEQ:
                case S_EMPTY:
                case S_CONCAT:
                    return '>';
                case S_MULDIV:
                case S_PARL:
                case S_VALUE:
                case S_GETLEN:
                    return '<';
                default:
                    return 'e';
            }

        case S_MULDIV:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARR:
                case S_COMPARE:
                case S_EQNEQ:
                case S_EMPTY:
                case S_CONCAT:
                    return '>';
                case S_PARL:
                case S_VALUE:
                case S_GETLEN:
                    return '<';
                default:
                    return 'e';
            }

        case S_PARL:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARL:
                case S_VALUE:
                case S_COMPARE:
                case S_EQNEQ:
                case S_EMPTY:
                case S_GETLEN:
                case S_CONCAT:
                    return '<';
                case S_PARR:
                    return '=';
                default:
                    return 'e';
            }

        case S_PARR:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARR:
                case S_COMPARE:
                case S_EQNEQ:
                case S_EMPTY:
                case S_CONCAT:
                    return '>';
                default:
                    return 'e';
            }

        case S_VALUE:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARR:
                case S_COMPARE:
                case S_EQNEQ:
                case S_EMPTY:
                case S_CONCAT:
                    return '>';
                default:
                    return 'e';
            }

        case S_COMPARE:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARL:
                case S_VALUE:
                case S_GETLEN:
                case S_CONCAT:
                    return '<';
                case S_PARR:
                case S_COMPARE:
                case S_EQNEQ:
                case S_EMPTY:
                    return '>';
                default:
                    return 'e';
            }

        case S_EQNEQ:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARL:
                case S_VALUE:
                case S_COMPARE:
                case S_GETLEN:
                    return '<';
                case S_PARR:
                case S_EQNEQ:
                case S_EMPTY:
                    return '>';
                default:
                    return 'e';
            }

        case S_GETLEN:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARR:
                case S_VALUE:
                case S_COMPARE:
                case S_EQNEQ:
                case S_GETLEN:
                case S_EMPTY:
                case S_CONCAT:
                    return '>';
                case S_PARL:
                    return '<';
                default:
                    return 'e';
            }

        case S_EMPTY:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARL:
                case S_VALUE:
                case S_COMPARE:
                case S_EQNEQ:
                case S_GETLEN:
                case S_CONCAT:
                    return '<';
                default:
                    return 'e';
            }

        case S_CONCAT:
            switch (in) {
                case S_ADDSUB:
                case S_MULDIV:
                case S_PARL:
                case S_VALUE:
                case S_EQNEQ:
                case S_EMPTY:
                case S_CONCAT:
                    return '>';
                case S_PARR:
                case S_COMPARE:
                case S_GETLEN:
                    return '<';
                default:
                    return 'e';
            }

        default:
            return 'e';
    }
}

void retype_to_float() {
    // retypes register a to float
    int skip = gen_new_label();
    gen_print("TYPE GF@b GF@a\n");
    gen_print("JUMPIFEQ %%%d GF@b string@float\n", skip);
    gen_print("JUMPIFEQ %%ERR_RT_NIL GF@b string@nil\n");
    gen_print("INT2FLOAT GF@a GF@a\n");
    gen_print("LABEL %%%d\n", skip);
}

Status reduce_value(SymStack *s) {
    Symbol x = symstack_pop(s);
    symstack_pop(s); // handle
    gen_print("PUSHS ");
    gen_print_value(x.token, st);// TODO all the type checks and code generation
    gen_print("\n");

    Type exprType = st_token_to_type(st, x.token);
    Symbol newExpr = { .symbolType = S_EXPR, .varType=exprType };
    scanner_destroy_token(&x.token);

    symstack_push(s, newExpr);
    return SUCCESS;
}

Status reduce_addsub(SymStack *s) {
    Symbol y = symstack_pop(s);
    Symbol op = symstack_pop(s);
    Symbol x = symstack_pop(s);
    symstack_pop(s); // handle

    Type resultType;

    bool xValid = (x.varType == INTEGER) || (x.varType == NUMBER);
    bool yValid = (y.varType == INTEGER) || (y.varType == NUMBER);
    if (xValid && yValid) {
        // input expressions have valid types
        // if they are both integers, we don't need implicit conversion
        if (x.varType == INTEGER && y.varType == INTEGER) {
            resultType = INTEGER;
        } else {
            // do the implicit conversion
            gen_print("POPS GF@a\n");
            retype_to_float();
            gen_print("MOVE GF@swap1 GF@a\n");
            gen_print("POPS GF@a\n");
            retype_to_float();
            gen_print("PUSHS GF@a\n");
            gen_print("PUSHS GF@swap1\n");

            resultType = NUMBER;
        }
    } else {
        return ERR_SEMANTIC_EXPR;
    }

    if (op.token.type == TOKEN_PLUS) {
        gen_print("ADDS\n");
    } else {
        gen_print("SUBS\n");
    }

    scanner_destroy_token(&op.token);

    Symbol newExpr = { .symbolType = S_EXPR, .varType=resultType };
    symstack_push(s, newExpr);
    return SUCCESS;
}

Status reduce_muldiv(SymStack *s) {
    Symbol y = symstack_pop(s);
    Symbol op = symstack_pop(s);
    Symbol x = symstack_pop(s);
    symstack_pop(s); // handle

    Type resultType;

    bool xValid = (x.varType == INTEGER) || (x.varType == NUMBER);
    bool yValid = (y.varType == INTEGER) || (y.varType == NUMBER);
    if (xValid && yValid) {
        // input expressions have valid types
        // if they are both integers, we don't need implicit conversion
        if (x.varType == INTEGER && y.varType == INTEGER) {
            resultType = INTEGER;
        } else {
            // do the implicit conversion
            gen_print("POPS GF@a\n");
            retype_to_float();
            gen_print("MOVE GF@swap1 GF@a\n");
            gen_print("POPS GF@a\n");
            retype_to_float();
            gen_print("PUSHS GF@a\n");
            gen_print("PUSHS GF@swap1\n");

            resultType = NUMBER;
        }
    } else {
        return ERR_SEMANTIC_EXPR;
    }

    if (op.token.type == TOKEN_MULTIPLY) {
        gen_print("MULS\n");
    } else {
        gen_print("POPS GF@a\n");
        // retypes register a to float
        int floatLabel = gen_new_label();
        int endLabel = gen_new_label();

        gen_print("TYPE GF@b GF@a\n");
        gen_print("JUMPIFEQ %%%d GF@b string@float\n", floatLabel);
        // check for int
        gen_print("JUMPIFEQ %%ERR_RT_DIVZERO GF@a int@0\n");
        // JMP end
        gen_print("JUMP %%%d\n", endLabel);
        // check for float
        gen_print("LABEL %%%d\n", floatLabel);
        gen_print("JUMPIFEQ %%ERR_RT_DIVZERO GF@a float@0x0p+0\n");
        // end
        gen_print("LABEL %%%d\n", endLabel);
        gen_print("PUSHS GF@a\n");

        gen_print("DIVS\n");
    }

    scanner_destroy_token(&op.token);

    Symbol newExpr = { .symbolType = S_EXPR, .varType=resultType };
    symstack_push(s, newExpr);
    return SUCCESS;
}

Status reduce_parenthesis(SymStack *s) {
    Symbol parLeft = symstack_pop(s);
    Symbol expr = symstack_pop(s);
    Symbol parRight = symstack_pop(s);
    symstack_pop(s); // handle
    // TODO all the type checks and code generation

    scanner_destroy_token(&parLeft.token);
    scanner_destroy_token(&parRight.token);

    symstack_push(s, expr);
    return SUCCESS;
}

Status reduce_get_length(SymStack *s) {
    Symbol expr = symstack_pop(s);
    Symbol hashtag = symstack_pop(s);
    symstack_pop(s); // handle

    if (expr.varType == STRING) {
        // TODO code generation
    }
    else {
        return ERR_SEMANTIC_EXPR;
    }

    scanner_destroy_token(&hashtag.token);

    Symbol newExpr = { .symbolType = S_EXPR, .varType=INTEGER };
    symstack_push(s, newExpr);
    return SUCCESS;
}

Status reduce_placeholder(SymStack *s) {
    Symbol y = symstack_pop(s);
    Symbol op = symstack_pop(s);
    Symbol x = symstack_pop(s);
    symstack_pop(s); // handle
    if (y.token.type == TOKEN_INTEGER_LIT && x.token.type != TOKEN_INTEGER_LIT )

    // TODO all the type checks and code generation

    (void)y;
    (void)x;
    (void)op;

    symstack_push(s, GENERIC_EXPR);
    return SUCCESS;
}

bool symstack_reduce(SymStack *s, Status *status) {
    // WARNING: all the rules are written in reverse order
    // It is easier to match them to stack top
    // This table is dependent on zero initialization of rules, that are not specified
    static const Rule ruleTable[] = {
            {.to={S_EXPR, S_GETLEN}, .fn=reduce_get_length},
            {.to={S_VALUE}, .fn=reduce_value},
            {.to={S_PARR, S_EXPR, S_PARL}, .fn=reduce_parenthesis},
            {.to={S_EXPR, S_ADDSUB, S_EXPR}, .fn=reduce_addsub},
            {.to={S_EXPR, S_MULDIV, S_EXPR}, .fn=reduce_muldiv},
            {.to={S_PARR, S_MULDIV, S_PARL}, .fn=reduce_parenthesis},   // TO DO idk if this ever happens
            {.to={S_EXPR, S_CONCAT, S_EXPR}, .fn=reduce_placeholder},
            {.to={S_EXPR, S_COMPARE, S_EXPR}, .fn=reduce_placeholder},
            {.to={S_EXPR, S_EQNEQ, S_EXPR}, .fn=reduce_placeholder},
    };
    int numRules = sizeof(ruleTable) / sizeof(Rule);

    for (int i=0; i<numRules; i++) {
        for (int j=0; j<MAX_RULE_LENGTH; j++) {
            SymbolType actual = symstack_peek(s, j).symbolType;
            SymbolType expected = ruleTable[i].to[j];
            if (actual != expected) {
                if ((expected == S_NONE) && (actual==S_HANDLE)) {
                    // we've made it to the end of the rule
                    // so we can apply its function
                    Status returnStatus = ruleTable[i].fn(s);
                    if (returnStatus != SUCCESS) {
                        *status = returnStatus;
                        return false;
                    }
                    return true;
                } else {
                    // move onto the next rule
                    break;
                }
            }
        }
    }
    return false;
}


bool nt_expr(Token *pToken, SymTab *symTab, Status *status, Type *returnType) {
    st = symTab;
    SymStack s;
    symstack_init(&s);

    symstack_push(&s, EMPTY_SYMBOL);

    bool ret = false;
    bool cont = true;

    while (cont) {
        Symbol stackTopTerminal = symstack_top_terminal(&s);
        Symbol inputTerminal = token_to_symbol(*pToken, st);

        if (inputTerminal.symbolType == S_NONE) {
            inputTerminal = EMPTY_SYMBOL;
        }

        if ((inputTerminal.symbolType == S_EMPTY)
            && (stackTopTerminal.symbolType == S_EMPTY)) {
            // The end of the expression, nothing left to do
            ret = true;
            break;
        }

        switch (table_lookup(stackTopTerminal, inputTerminal)) {
            case '=':
                // push the incoming symbol onto the stack
                symstack_push(&s, inputTerminal);
                *status = scanner_get_token(pToken);
                if (*status != SUCCESS) {
                    cont = false;
                    ret = false;
                    break;
                }
                continue;

            case '<':
                // do the shift operation
                symstack_push_above_top_terminal(&s, HANDLE);
                symstack_push(&s, inputTerminal);

                // get new token
                *status = scanner_get_token(pToken);
                if (*status != SUCCESS) {
                    cont = false;
                    ret = false;
                    break;
                }
                continue;

            case '>':
                if (!symstack_reduce(&s, status)) {
                    cont = false;
                    ret = false;
                    break;
                }
                // do not get a new token
                continue;

            default:;
                // The table does not allow this combination
                // the cycle will be ended
                cont = false;
                // filthy hack, might need to be fixed later
                while (symstack_reduce(&s, status)) {
                    // reduce
                }
                // TODO definitely needs refactorin
                // this is basically the same condition as the first one
                if (symstack_top(&s).symbolType != S_EXPR) {
                    ret = false;
                    break;
                }
                if (symstack_peek(&s, 1).symbolType != S_EMPTY) {
                    ret = false;
                    break;
                }
                ret = true;
        }
    }

    if (returnType) {
        // Assign the final expression's type
        *returnType = symstack_top(&s).varType;
    }

    symstack_destroy(&s);
    return ret;
}
