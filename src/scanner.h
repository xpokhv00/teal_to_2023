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
    NONE,
    TOKEN_IDENTIFIER,
    TOKEN_EOF,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_DIVIDE,
    TOKEN_INT_DIVIDE,
    TOKEN_MULTIPLY,
    TOKEN_EQUALS,

    // literals
    TOKEN_INTEGER_LIT,
    TOKEN_DOUBLE_LIT,
    TOKEN_STRING_LIT,

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
    TOKEN_INTEGER_KW,
    TOKEN_LOCAL,
    TOKEN_NIL,
    TOKEN_NUMBER_KW,
    TOKEN_REQUIRE,
    TOKEN_RETURN,
    TOKEN_STRING_KW,
    TOKEN_THEN,
    TOKEN_WHILE,

    TOKEN_CONCATENATE,
    TOKEN_GET_LENGTH,
    TOKEN_ASSIGN,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_PAR_L,
    TOKEN_PAR_R,

    TOKEN_ERROR, // TODO what is this, is it really needed?

    NUM_TOKENS, // This must be the last item
} TokenType;


typedef struct {
    char *str;
    TokenType type;
    // Location of the token in the input file
    // these are just for debugging purposes
    unsigned lineNumber;
    unsigned characterNumber;
} Token;


typedef enum {
    SCANNER_START,

    SCANNER_STRING_START,
    SCANNER_STRING_VALUE,
    SCANNER_STRING_FINAL,
    SCANNER_ESC_SEQ,
    SCANNER_SEQ_0,
    SCANNER_SEQ_00,
    SCANNER_SEQ_1,
    SCANNER_SEQ_DEF,
    SCANNER_SEQ_2,
    SCANNER_SEQ_25,

    SCANNER_LT,
    SCANNER_LEQ,
    SCANNER_GT,
    SCANNER_GTE,
    SCANNER_EQ,
    SCANNER_NEQ,
    SCANNER_TILDA,
    SCANNER_ONE_EQUALS,

    SCANNER_PLUS,
    SCANNER_MINUS,
    SCANNER_DIVIDE,
    SCANNER_INT_DIVIDE,
    SCANNER_MULTIPLY,
    SCANNER_COLON, // ":"
    SCANNER_ASSIGN,
    SCANNER_COMMA,
    SCANNER_PAR_L,
    SCANNER_PAR_R,

    SCANNER_INT,
    SCANNER_NUMBER_POINT,
    SCANNER_DOUBLE,
    SCANNER_EXP_BASE,
    SCANNER_EXP_SIGN,
    SCANNER_EXP,

    SCANNER_DOT,
    SCANNER_CONCATENATE,
    SCANNER_GET_LENGTH,

    SCANNER_IDENTIFIER,
    SCANNER_CHECK_IDENTIFIER,

    SCANNER_COMMENT_START,
    SCANNER_ALMOST_BLOCK,
    SCANNER_COMMENT_BLOCK,
    SCANNER_ALMOST_END,
    SCANNER_COMMENT_END,
    SCANNER_COMMENT,

    SCANNER_EOL,
    SCANNER_ERROR,
    SCANNER_EOF
} ScannerState;

Status scanner_init(FILE *in);

Status scanner_get_token(Token *pToken);

void scanner_destroy_token(Token *pToken);

Status scanner_destroy();

#endif // SCANNER_H
