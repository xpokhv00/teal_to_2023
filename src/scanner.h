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
    NONE = 0, // This is needed for the implementation of LL-table
    TOKEN_IDENTIFIER,
    TOKEN_EOF,
    TOKEN_EOL,
    TOKEN_KEYWORD,
    TOKEN_COMMENT,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIVIDE,
    TOKEN_INT_DIVIDE,
    TOKEN_MULTIPLY,
    TOKEN_EQUALS,

    TOKEN_INT,
    TOKEN_DOUBLE,
    TOKEN_STRING,
    TOKEN_EXP,

    TOKEN_LT,
    TOKEN_LEQ,
    TOKEN_GT,
    TOKEN_GEQ,
    TOKEN_NEQ,

    // keyword tokens
    TOKEN_DO,
    TOKEN_ELSE,
    TOKEN_END,
    TOKEN_FUNCTION,
    TOKEN_GLOBAL,
    TOKEN_IF,
    TOKEN_INTEGER,
    TOKEN_LOCAL,
    TOKEN_NIL,
    TOKEN_NUMBER,
    TOKEN_REQUIRE,
    TOKEN_RETURN,
    TOKEN_KEYWORD_STRING,
    TOKEN_THEN,
    TOKEN_WHILE,

    TOKEN_CONCATENATE,
    TOKEN_GET_LENGTH,
    TOKEN_ASSIGN,
    // zmenit
    TOKEN_DVOJTECKAASI,
    TOKEN_ERROR,

    // TODO add more types
    NUM_TOKENS, // C dark magic, please keep this at the end
    // non-terminals go here
    NT_PROG,
    NT_PROLOG,
    NT_PROG_BODY,
    NT_BODY,
    NT_FN_DECL,
    NT_FN_DEF,
    NT_IF,
    NT_WHILE,

} TokenType;

typedef enum { // TODO this should be part of token type
    KEYWORD_DO,
    KEYWORD_ELSE,
    KEYWORD_END,
    KEYWORD_FUNCTION,
    KEYWORD_GLOBAL,
    KEYWORD_IF,
    KEYWORD_INTEGER,
    KEYWORD_LOCAL,
    KEYWORD_NIL,
    KEYWORD_NUMBER,
    KEYWORD_REQUIRE,
    KEYWORD_RETURN,
    KEYWORD_STRING,
    KEYWORD_THEN,
    KEYWORD_WHILE
} Keyword;

typedef struct {
    char *str;
    TokenType type;
    // Location of the token in the input file
    // these are just for debugging purposes
    unsigned lineNumber;
    unsigned characterNumber;
    Keyword keyword; // TODO no keyword here, it will be in type variable
} Token;


// TODO this should probably be enum
#define SCANNER_START 1
#define SCANNER_STRING 2
#define SCANNER_NUMBER 3
#define SCANNER_COMMENTARY 4
#define SCANNER_BLOCK_COMMENTARY 5
#define SCANNER_ESCAPE_SEQ 6
#define SCANNER_EOL 7
#define SCANNER_SINGLE_OPERATOR 8
#define SCANNER_END 9
#define SCANNER_ERROR 10
#define SCANNER_DOUBLE_OPERATOR 11
#define SCANNER_LINE_COMMENTARY 12


Status scanner_init(FILE *in);

Status scanner_get_token(Token *pToken);

Status scanner_destroy();

#endif // SCANNER_H
