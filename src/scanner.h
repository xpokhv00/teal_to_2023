/**
 * @file scanner.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */


#ifndef SCANNER_H
#define SCANNER_H

#include "common.h"
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    TOKEN_IDENTIFIER,
    // TODO add more types
} TokenType;

typedef struct {
    char *str;
    TokenType type;
    // Location of the token in the input file
    // these are just for debugging purposes
    unsigned lineNumber;
    unsigned characterNumber;
} Token;


Status scanner_init(FILE *in);

Status scanner_get_token(Token *pToken, bool end);

Status scanner_destroy();

#endif // SCANNER_H
