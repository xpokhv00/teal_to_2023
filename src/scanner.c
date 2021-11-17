/**
 * @file scanner.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "scanner.h"

// The file, that is being read
static FILE *inFile;

// Count lines from the start of file and store it in tokens
// this is for easier debugging
static unsigned lineCount;
// characters form the start of the line
static unsigned characterCount;
// size of malloc/realloc
unsigned alloc_size = 500;

// if a letter is read after the token has ended, it is kept here
// until the next reading
//static int nextLetter;

void str_realloc(Token *pToken) {
    alloc_size *= 2;
    pToken->str = realloc(pToken->str, alloc_size);
}


Status scanner_init(FILE *in) {
    inFile = in;
    return SUCCESS;
}


Status scanner_get_token(Token *pToken) {

    // realloc doubles this size
    unsigned alloc_size = 500;
    unsigned str_index = 0;

   pToken = malloc(sizeof(Token));

   pToken->str = malloc(sizeof(char) * alloc_size);
   if (pToken == NULL || pToken->str == NULL) {
      return ERR_INTERNAL;
   }

    memset(pToken->str, 0, sizeof(char)*(alloc_size-1));
    if (inFile == NULL) {
        return ERR_INTERNAL;
    }

    int state = SCANNER_START;
    int input;

    pToken->characterNumber = characterCount;
    pToken->lineNumber = lineCount;
    while (true) {
        input = getc(inFile);
        if (strlen(pToken->str) > alloc_size - 10) {
            str_realloc(pToken);
        }
        if (pToken->str == NULL) {
            return ERR_INTERNAL;
        }

        pToken->str[str_index++] = input;

        switch (state) {
            case SCANNER_START:
                characterCount++;
                if (input == '\n') {
                    lineCount++;
                    state = SCANNER_EOL;
                } else if (input == ':') {
                    state = SCANNER_COLON;
                    pToken->type = TOKEN_COLON;
                } else if (input == '+') {
                    state = SCANNER_PLUS;
                    pToken->type = TOKEN_PLUS;
                } else if (input == '-') {
                    state = SCANNER_MINUS;
                    pToken->type = TOKEN_MINUS;
                } else if (input == '*') {
                    state = SCANNER_MULTIPLY;
                    pToken->type = TOKEN_MULTIPLY;
                } else if (input == '/') {
                    state = SCANNER_DIVIDE;
                    pToken->type = TOKEN_DIVIDE;
                } else if (input == '.') {
                    state = SCANNER_DOT;
                } else if (input == '>') {
                    state = SCANNER_GT;
                    pToken->type = TOKEN_GT;
                } else if (input == '<') {
                    state = SCANNER_LT;
                    pToken->type = TOKEN_LT;
                } else if (input == '~') {
                    state = SCANNER_TILDA;
                } else if (input == '=') {
                    state = SCANNER_ONE_EQUALS;
                } else if (input == '#') {
                    state = SCANNER_GET_LENGTH;
                    pToken->type = TOKEN_GET_LENGTH;
                } else if (input == '\"') {
                    state = SCANNER_STRING_START;
                    pToken->type = TOKEN_STRING_LIT;
                } else if (isspace(input)) {
                    continue;
                } else if (input >= '0' && input <= '9') {
                    state = SCANNER_INT;
                } else if ((input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z') || input == '_') {
                    state = SCANNER_IDENTIFIER;
                } else if (input == EOF) {
                    pToken->type = TOKEN_EOF;
                    state = SCANNER_EOF;
                }else if(input == ','){
                    state = SCANNER_COMMA;
                    pToken->type = TOKEN_COMMA;
                }else if(input == '('){
                    state = SCANNER_PAR_L;
                    pToken->type = TOKEN_PAR_L;
                }else if(input == ')'){
                    state = SCANNER_PAR_R;
                    pToken->type = TOKEN_PAR_R;
                }
                break;

            case SCANNER_PAR_L:
            case SCANNER_PAR_R:
            case SCANNER_COMMA:
                ungetc(input, inFile);
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;
            case SCANNER_STRING_START:
                if (input != '"' && input > 31) {
                    state = SCANNER_STRING_VALUE;
                } else if (input == '"') {
                    state = SCANNER_STRING_FINAL;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_STRING_VALUE:
                if (input == '\\') {
                    state = SCANNER_ESC_SEQ;
                } else if (input == '"') {
                    state = SCANNER_STRING_FINAL;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_STRING_FINAL:
                pToken->type = TOKEN_STRING_LIT;
                str_index = 0;
                ungetc(input, inFile);
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;
            case SCANNER_ESC_SEQ:
                if (input == '0') {
                    state = SCANNER_SEQ_0;
                } else if (input == '1') {
                    state = SCANNER_SEQ_1;
                } else if (input == '2') {
                    state = SCANNER_SEQ_2;
                } else if (input == 't' || input == 'n' || input == '"' || input == '/') {
                    state = SCANNER_STRING_VALUE;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_0:
                if (input == '0') {
                    state = SCANNER_SEQ_00;
                } else if (input > '0' && input <= '9') {
                    state = SCANNER_SEQ_DEF;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_1:
                if (input >= '0' && input <= '9') {
                    state = SCANNER_SEQ_DEF;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_2:
                if (input == '5') {
                    state = SCANNER_SEQ_25;
                } else if (input >= '0' && input <= '4') {
                    state = SCANNER_SEQ_DEF;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_00:
                if (input >= '1' && input <= '9') {
                    state = SCANNER_STRING_VALUE;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_DEF:
                if (input >= '0' && input <= '9') {
                    state = SCANNER_STRING_VALUE;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_25:
                if (input >= '0' && input <= '5') {
                    state = SCANNER_STRING_VALUE;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;

            case SCANNER_MINUS:
                if (input == '-') {
                    state = SCANNER_COMMENT_START;
                    str_index = 0;
                } else {
                    ungetc(input, inFile);
                    pToken->str[(str_index-1)] = '\0';
                    return SUCCESS;
                }
                characterCount++;
                break;
            case SCANNER_PLUS:
                ungetc(input, inFile);
                return SUCCESS;
            case SCANNER_DIVIDE:
                if (input == '/') {
                    state = SCANNER_INT_DIVIDE;
                    break;
                } else {
                    ungetc(input, inFile);
                    pToken->str[(str_index-1)] = '\0';
                    return SUCCESS;
                }
            case SCANNER_INT_DIVIDE:
                ungetc(input, inFile);
                pToken->type = TOKEN_INT_DIVIDE;
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;
            case SCANNER_MULTIPLY:
                ungetc(input, inFile);
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;
            case SCANNER_DOT:
                if (input == '.') {
                    characterCount++;
                    state = SCANNER_CONCATENATE;
                    pToken->type = TOKEN_CONCATENATE;
                } else {
                    state = SCANNER_ERROR;
                    ungetc(input, inFile);
                }
                break;
            case SCANNER_CONCATENATE:
                ungetc(input, inFile);
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;
            case SCANNER_COLON:
                ungetc(input, inFile);
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;

            case SCANNER_ONE_EQUALS:
                if (input == '=') {
                    state = SCANNER_EQ;
                    pToken->type = TOKEN_EQUALS;
                    characterCount++;
                } else {
                    state = SCANNER_ASSIGN;
                    pToken->type = TOKEN_ASSIGN;
                    ungetc(input, inFile);
                }
                break;
            case SCANNER_EQ:
                ungetc(input, inFile);
                return SUCCESS;

            case SCANNER_LT:
                if (input == '=') {
                    characterCount++;
                    state = SCANNER_LEQ;
                    break;
                } else {
                    ungetc(input, inFile);
                    pToken->str[(str_index-1)] = '\0';
                    return SUCCESS;
                }
            case SCANNER_GT:
                if (input == '=') {
                    state = SCANNER_GTE;
                    characterCount++;
                    break;
                } else {
                    ungetc(input, inFile);
                    pToken->str[(str_index-1)] = '\0';
                    return SUCCESS;
                }
            case SCANNER_LEQ:
                ungetc(input, inFile);
                pToken->type = TOKEN_LEQ;
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;
            case SCANNER_GTE:
                ungetc(input, inFile);
                pToken->type = TOKEN_GEQ;
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;
            case SCANNER_TILDA:
                if (input == '=') {
                    pToken->type = TOKEN_NEQ;
                    state = SCANNER_NEQ;
                } else {
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_NEQ:
                ungetc(input, inFile);
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;

            case SCANNER_GET_LENGTH:
                ungetc(input, inFile);
                pToken->str[(str_index-1)] = '\0';
                return SUCCESS;

            case SCANNER_COMMENT_START:
                if (input == '\n') {
                    state = SCANNER_COMMENT_END;
                } else if (input == '[') {
                    state = SCANNER_ALMOST_BLOCK;
                } else {
                    state = SCANNER_COMMENT;
                }
                break;
            case SCANNER_ALMOST_BLOCK:
                if (input == '[') {
                    state = SCANNER_COMMENT_BLOCK;
                } else {
                    state = SCANNER_COMMENT;
                }
                break;
            case SCANNER_COMMENT:
                if (input == '\n') {
                    state = SCANNER_COMMENT_END;
                }
                break;
            case SCANNER_COMMENT_END:
                ungetc(input, inFile);
                state = SCANNER_START;
                break;
            case SCANNER_COMMENT_BLOCK:
                if (input == ']') {
                    state = SCANNER_ALMOST_END;
                }
                break;
            case SCANNER_ALMOST_END:
                if (input == ']') {
                    state = SCANNER_COMMENT_END;
                } else {
                    state = SCANNER_COMMENT_BLOCK;
                }
                break;

            case SCANNER_EOL:
                ungetc(input, inFile);
                lineCount += 1;
                characterCount = 0;
                state = SCANNER_START;
                break;

            case SCANNER_INT:
                if (input >= '0' && input <= '9') {

                } else if (input == '.') {
                    state = SCANNER_NUMBER_POINT;
                } else {
                    ungetc(input, inFile);
                    characterCount++;
                    pToken->type = TOKEN_INTEGER_LIT;
                    pToken->str[(str_index-1)] = '\0';
                    return SUCCESS;
                }
                characterCount++;
                break;

            case SCANNER_NUMBER_POINT:
                if (input == 'e' || input == 'E') {
                    characterCount++;
                    state = SCANNER_EXP_BASE;
                } else if (input >= '0' && input <= '9') {
                    state = SCANNER_DOUBLE;
                    characterCount++;
                } else {
                    ungetc(input, inFile);
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_DOUBLE:
                if (input >= '0' && input <= '9') {
                    characterCount++;
                } else if (input == 'e' || input == 'E') {
                    characterCount++;
                    state = SCANNER_EXP_BASE;
                } else {
                    ungetc(input, inFile);
                    characterCount++;
                    pToken->type = TOKEN_DOUBLE_LIT;
                    pToken->str[(str_index-1)] = '\0';
                    return SUCCESS;
                }
                break;
            case SCANNER_EXP_BASE:
                if (input >= '0' && input <= '9') {
                    characterCount++;
                    state = SCANNER_EXP;
                } else if (input == '+' || input == '-') {
                    state = SCANNER_EXP_SIGN;
                } else {
                    ungetc(input, inFile);
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_EXP_SIGN:
                if (input >= '0' && input <= '9') {
                    characterCount++;
                    state = SCANNER_EXP;
                } else {
                    ungetc(input, inFile);
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_EXP:
                if (input >= '0' && input <= '9') {
                    characterCount++;
                } else {
                    ungetc(input, inFile);
                    characterCount++;
                    pToken->type = TOKEN_DOUBLE_LIT;
                    pToken->str[(str_index-1)] = '\0';
                    return SUCCESS;
                }
                break;

            case SCANNER_IDENTIFIER:
                if ((input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z') || input == '_' ||
                    (input >= '0' && input <= '9')) {
                }else{
                    pToken->str[(str_index-1)] = '\0';
                    ungetc(input, inFile);
                    state = SCANNER_CHECK_IDENTIFIER;
                }
                break;

            case SCANNER_CHECK_IDENTIFIER:
                if (!strcmp(pToken->str, "do")) {
                    pToken->type = TOKEN_DO;
                } else if (!strcmp(pToken->str, "else")) {
                    pToken->type = TOKEN_ELSE;
                } else if (!strcmp(pToken->str, "end")) {
                    pToken->type = TOKEN_END;
                } else if (!strcmp(pToken->str, "function")) {
                    pToken->type = TOKEN_FUNCTION;
                } else if (!strcmp(pToken->str, "global")) {
                    pToken->type = TOKEN_GLOBAL;
                } else if (!strcmp(pToken->str, "if")) {
                    pToken->type = TOKEN_IF;
                } else if (!strcmp(pToken->str, "integer")) {
                    pToken->type = TOKEN_INTEGER_KW;
                } else if (!strcmp(pToken->str, "local")) {
                    pToken->type = TOKEN_LOCAL;
                } else if (!strcmp(pToken->str, "nil")) {
                    pToken->type = TOKEN_NIL;
                } else if (!strcmp(pToken->str, "number")) {
                    pToken->type = TOKEN_NUMBER_KW;
                } else if (!strcmp(pToken->str, "require")) {
                    pToken->type = TOKEN_REQUIRE;
                } else if (!strcmp(pToken->str, "return")) {
                    pToken->type = TOKEN_RETURN;
                } else if (!strcmp(pToken->str, "string")) {
                    pToken->type = TOKEN_STRING_KW;
                } else if (!strcmp(pToken->str, "then")) {
                    pToken->type = TOKEN_THEN;
                } else if (!strcmp(pToken->str, "while")) {
                    pToken->type = TOKEN_WHILE;
                } else {
                    pToken->type = TOKEN_IDENTIFIER;
                }
                return SUCCESS;

            case SCANNER_EOF:
                return state;

            default:
            case SCANNER_ERROR:
                str_index = 0;
                ungetc(input, inFile);
                return ERR_LEXICAL;
        }
    }
}

void scanner_destroy_token(Token *pToken) {
    free(pToken->str);
}

Status scanner_destroy() {
    // deallocate all the resources
    fclose(inFile);
    return SUCCESS;
}

