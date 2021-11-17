/**
 * @file common.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

typedef enum {
    SUCCESS = 0,
    ERR_LEXICAL = 1,
    ERR_SYNTAX = 2,
    ERR_SEMANTIC_DEF = 3,
    ERR_SEMANTIC_ASSIGN = 4,
    ERR_SEMANTIC_FUNC = 5,
    ERR_SEMANTIC_EXPR = 6,
    ERR_SEMANTIC_OTHER = 7,
    ERR_RUNTIME_NIL = 8,
    ERR_RUNTIME_DIV_ZERO = 9,
    ERR_INTERNAL = 99,
} Status;

#endif // COMMON_H
