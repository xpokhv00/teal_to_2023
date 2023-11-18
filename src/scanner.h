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
    TYPE_EOF,
    TYPE_PLUS,
    TYPE_MINUS,

    TYPE_LT,
    TYPE_LEQ,
    TYPE_GT,
    TYPE_GEQ,
    TYPE_NEQ,

    // keyword tokens
    TYPE_DO,
    TYPE_ELSE,
    TYPE_END,
    TYPE_FUNCTION,
    TYPE_REQUIRE,
    TYPE_GLOBAL,
    TYPE_LET,
    TYPE_VAR,
    TYPE_IF,
    TYPE_INTEGER_KW,
    TYPE_LOCAL,
    TYPE_NIL,
    TYPE_NUMBER_KW,
    TYPE_RETURN,
    TYPE_STRING_KW,
    TYPE_THEN,
    TYPE_WHILE,

    TYPE_CONCATENATE,
    TYPE_STRING_LIT,
    TYPE_INTEGER_LIT,
    TYPE_DOUBLE_LIT,
    TYPE_ASSIGN,
    TYPE_COLON,
    TYPE_COMMA,
    TYPE_PAR_L,
    TYPE_PAR_R,

    // mine

    TYPE_KW,
    TYPE_ID,
    TYPE_EOL,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_LESS,
    TYPE_MORE,
    TYPE_LESS_EQUAL,
    TYPE_MORE_EQUAL,
    TYPE_EQUAL,
    TYPE_NOT_EQUAL,
    TYPE_LEFT_BRACKET,
    TYPE_RIGHT_BRACKET,
    TYPE_EXCLAMATION_MARK,
    TYPE_LEFT_CURLY_BRACKET,
    TYPE_RIGHT_CURLY_BRACKET,
    TYPE_RETURN_ARROW,
    TYPE_NIL_COALESCING_OPERATOR,
    TYPE_MULTILINE_STRING,
    TYPE_ERROR,
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
    SCANNER_COLON,
    SCANNER_COMMA,
    SCANNER_PAR_L,
    SCANNER_PAR_R,
    SCANNER_CUR_PAR_L,
    SCANNER_CUR_PAR_R,

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

    SCANNER_COMMENT_START,
    SCANNER_ALMOST_BLOCK,
    SCANNER_COMMENT_BLOCK,
    SCANNER_ALMOST_END,
    SCANNER_COMMENT,

    SCANNER_EOF
} ScannerState;

Status scanner_init(FILE *in);
Status scanner_get_token(Token *pToken);
void scanner_unget_token(Token token);
void scanner_destroy_token(Token *pToken);
Status scanner_destroy();

#endif // SCANNER_H
