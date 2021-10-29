/**
 * @file parser.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */


#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "scanner.h"
#include "syntaxstack.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// one non-terminal can be rewritten to this many terminals or non-terminals
// needed only for practical implementation of LL table
#define MAX_RULE_LENGTH 10

typedef struct {
    TokenType type;
    bool empty;
    TokenType first[NUM_TOKENS];
    TokenType follow;
} LL_line;

typedef struct {
    TokenType from;
    TokenType to[MAX_RULE_LENGTH];
} Rule;


Status parser_init();

Status parser_run();

Status parser_destroy();

#endif // PARSER_H
