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

static SyntaxStack stack;

Status parser_init() {
    syntaxstack_init(&stack);
    return SUCCESS;
}

Status parser_run() {

    return SUCCESS;
}

Status parser_destroy() {
    syntaxstack_destroy(&stack);
    return SUCCESS;
}