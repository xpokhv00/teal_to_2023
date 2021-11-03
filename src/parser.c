/**
 * @file parser.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "parser.h"


// The numbering of non-terminals start after the terminals
// we need the table to start at index 0, so we do not waste too much memory
#define shift (NUM_TOKENS + 1)

/*
 * Gives the number of the rule, that should be applied
 * returns zero in case that no rule applies
 * @param[out] rule The index of the applied rule in LL-table
 * @param[in] top The top element of the syntax stack
 * @param[in] in The top element of the syntax stack
 */
int ll_lookup(SymbolType top, SymbolType first) {
    // The first index is array of all terminals
    // for each non-terminal on the top of the stack
    static const int llTable[][NUM_TOKENS] = {
            // this table relies on C array initialization
            // All fields, that do not have explicit value (rule number)
            // are zero
            [NT_PROG-shift] = {[TOKEN_IDENTIFIER]=1, [TOKEN_FUNCTION]=2, [TOKEN_LOCAL]=5},
            [NT_IF-shift] = {[TOKEN_IF]=7},
            [NT_WHILE-shift] = {[TOKEN_WHILE]=9},
    };

    // if there is a terminal on the top of the stack
    if (top < NUM_TOKENS) {
        // this function is not designed to deal with terminals
        return 0;
    }
    // return the corresponding rule number from the table
    return llTable[top-shift][first];
}

Status apply_rule(SyntaxStack *s, SymbolType first) {
    static const Rule ruleTable[] = {
            // this table relies on c array initialization
            // zero translates to SymbolType NONE

            // TODO only for illustrative purposes, probably needs to be rewritten
            [1] = {.from=NT_PROG, .to={NT_PROLOG, NT_PROG_BODY}},
            [2] = {.from=NT_PROG_BODY, .to={NT_FN_DECL, NT_PROG_BODY}},
            [3] = {.from=NT_PROG_BODY, .to={NT_FN_DEF, NT_PROG_BODY}},
            [4] = {.from=NT_PROG_BODY, .to={TOKEN_EOF}},
            [5] = {.from=NT_PROLOG, .to={TOKEN_REQUIRE, TOKEN_STRING}},
    };

    SymbolType top = syntaxstack_pop(s);

    int rule = ll_lookup(top, first);
    // if no rule applies, the syntax must be invalid
    if (rule == 0) {
        return ERR_SYNTAX;
    }

    if (top != ruleTable[rule].from) {
        // this should absolutely never happen
        // LL table must be wrong
        return ERR_INTERNAL;
    }

    // push all the new items onto the stack in reversed order
    for (int i=MAX_RULE_LENGTH-1; i>=0; i--) {
        SymbolType newItem = ruleTable[rule].to[i];
        if (newItem != NONE) {
            syntaxstack_push(s, newItem);
        }
    }
    return SUCCESS;
}

Status parser_init() {
    return SUCCESS;
}

Status parser_run() {
    SyntaxStack stack;
    syntaxstack_init(&stack);

    // Push starting non-terminal, that represents the entire program
    syntaxstack_push(&stack, NT_PROG);
    // read the first token
    Token token;
    Status status = scanner_get_token(&token);
    if (status != SUCCESS) {
        return status;
    }

    while (!syntaxstack_is_empty(&stack)) {
        // while the top of the stack matches with the type of the token
        // cancel both of them out and move onto the next pair
        while (syntaxstack_top(&stack) == token.type) {
            syntaxstack_pop(&stack);
            status = scanner_get_token(&token);
            if (status != SUCCESS) {
                return status;
            }
        }

        // when they do not match, try to apply a rule
        status = apply_rule(&stack, token.type);
        if (status != SUCCESS) {
            return status;
        }
    }

    syntaxstack_destroy(&stack);
    return SUCCESS;
}

Status parser_destroy() {
    return SUCCESS;
}