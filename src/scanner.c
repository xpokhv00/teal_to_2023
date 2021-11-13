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

int handleIdentifier(Token *pToken, char input) {
    int i = 1;
    while (true) {
        if (strlen(pToken->str) > alloc_size - 10) {
            str_realloc(pToken);
            printf("i did this shit");
        }
        if (pToken->str == NULL) {
            return ERR_INTERNAL;
        }

        input = getc(inFile);

        // TODO no magic numbers, use character literals like 'a' or '7'
        if ((input >= 65 && input <= 90) || (input >= 97 && input <= 122) || input == '_' ||
            (input >= 48 && input <= 57)) {
            pToken->str[i++] = input;
        } else if (isspace(input)) {
            break;
        } else {
            pToken->type = TOKEN_ERROR;
            return -2;
        }
    }

    if (!strcmp(pToken->str, "do")) {
        pToken->type = TOKEN_DO;
        return i;
    } else if (!strcmp(pToken->str, "else")) {
        pToken->type = TOKEN_ELSE;
        return i;
    } else if (!strcmp(pToken->str, "end")) {
        pToken->type = TOKEN_END;
        return i;
    } else if (!strcmp(pToken->str, "function")) {
        pToken->type = TOKEN_FUNCTION;
        return i;
    } else if (!strcmp(pToken->str, "global")) {
        pToken->type = TOKEN_GLOBAL;
        return i;
    } else if (!strcmp(pToken->str, "if")) {
        pToken->type = TOKEN_IF;
        return i;
    } else if (!strcmp(pToken->str, "integer")) {
        pToken->type = TOKEN_INTEGER_KW;
        return i;
    } else if (!strcmp(pToken->str, "local")) {
        pToken->type = TOKEN_LOCAL;
        return i;
    } else if (!strcmp(pToken->str, "nil")) {
        pToken->type = TOKEN_NIL;
        return i;
    } else if (!strcmp(pToken->str, "number")) {
        pToken->type = TOKEN_NUMBER_KW;
        return i;
    } else if (!strcmp(pToken->str, "require")) {
        pToken->type = TOKEN_REQUIRE;
        return i;
    } else if (!strcmp(pToken->str, "return")) {
        pToken->type = TOKEN_RETURN;
        return i;
    } else if (!strcmp(pToken->str, "string")) {
        pToken->type = TOKEN_STRING_LIT;
        return i;
    } else if (!strcmp(pToken->str, "then")) {
        pToken->type = TOKEN_THEN;
        return i;
    } else if (!strcmp(pToken->str, "while")) {
        pToken->type = TOKEN_WHILE;
        return i;
    } else {
        pToken->type = TOKEN_IDENTIFIER;
        return i;
    }
}

Status scanner_init(FILE *in) {
    inFile = in;
    return SUCCESS;
}


Status scanner_get_token(Token *pToken) {

    // realloc doubles this size
    unsigned alloc_size = 500;
    unsigned str_index = 0;

   // pToken = malloc(sizeof(Token));

   // pToken->str = malloc(sizeof(char) * alloc_size);
    //if (pToken == NULL || pToken->str == NULL) {
      //  return ERR_INTERNAL;
    //}

    memset(pToken->str, 0, sizeof(char)*(alloc_size-1));
    if (inFile == NULL) {
        return ERR_INTERNAL;
    }

    int state = SCANNER_START;
    int input;

    while (true) {
        input = getc(inFile);
        if (strlen(pToken->str) > alloc_size - 10) {
            str_realloc(pToken);
            printf("sheesh");
        }
        if (pToken->str == NULL) {
            return ERR_INTERNAL;
        }

        switch (state) {
            case SCANNER_START:
                if (input == '\n') {
                    lineCount++;
                    state = SCANNER_EOL;
                } else if (input == ':') {
                    state = SCANNER_COLON;
                    pToken->type = TOKEN_COLON;
                    characterCount++;

                } else if (input == '+') {
                    state = SCANNER_PLUS;
                    pToken->type = TOKEN_PLUS;
                    characterCount++;

                } else if (input == '-') {
                    state = SCANNER_MINUS;
                    pToken->type = TOKEN_MINUS;
                    characterCount++;
                } else if (input == '*') {
                    state = SCANNER_MULTIPLY;
                    pToken->type = TOKEN_MULTIPLY;
                    characterCount++;

                } else if (input == '/') {
                    state = SCANNER_DIVIDE;
                    pToken->type = TOKEN_DIVIDE;
                    characterCount++;

                } else if (input == '.') {
                    state = SCANNER_DOT;
                    characterCount++;

                } else if (input == '>') {
                    state = SCANNER_GT;
                    pToken->type = TOKEN_GT;
                    characterCount++;
                } else if (input == '<') {
                    state = SCANNER_LT;
                    pToken->type = TOKEN_LT;
                    characterCount++;

                } else if (input == '~') {
                    state = SCANNER_TILDA;
                    characterCount++;
                } else if (input == '=') {
                    state = SCANNER_ONE_EQUALS;
                    characterCount++;
                } else if (input == '#') {
                    state = SCANNER_GET_LENGTH;
                    pToken->type = TOKEN_GET_LENGTH;
                    characterCount++;
                } else if (input == '\"') {
                    state = SCANNER_STRING_START;
                    pToken->type = TOKEN_STRING_LIT;
                } else if (isspace(input)) {
                    continue;
                } else if (input >= 48 && input <= 57) {
                    state = SCANNER_INT;
                    characterCount++;
                    pToken->str[str_index++] = input;

                } else if ((input >= 65 && input <= 90) || (input >= 97 && input <= 122) || input == '_') {
                    pToken->str[0] = input;
                    state = SCANNER_IDENTIFIER;
                    characterCount++;
                } else if (input == EOF) {
                    pToken->type = TOKEN_EOF;
                    state = SCANNER_EOF;
                    pToken->lineNumber = lineCount;
                    pToken->characterNumber = characterCount;
                }else if(input == ','){
                    state = SCANNER_COMMA;
                    pToken->type = TOKEN_COMMA;
                    characterCount++;

                }else if(input == '('){
                    state = SCANNER_PAR_L;
                    pToken->type = TOKEN_PAR_L;
                    characterCount++;
                }else if(input == ')'){
                    state = SCANNER_PAR_R;
                    pToken->type = TOKEN_PAR_R;
                    characterCount++;
                }
                break;

            case SCANNER_PAR_L:
                pToken->str[0] = '(';
                pToken->characterNumber = characterCount;
                pToken->lineNumber = lineCount;
                ungetc(input, inFile);
                state = SUCCESS;
                return state;
            case SCANNER_PAR_R:
                pToken->str[0] = ')';
                pToken->characterNumber = characterCount;
                pToken->lineNumber = lineCount;
                ungetc(input, inFile);
                state = SUCCESS;
                return state;
            case SCANNER_COMMA:
                ungetc(input, inFile);
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;
            case SCANNER_STRING_START:
                if (input != '"' && input > 31) {
                    pToken->str[str_index++] = input;
                    state = SCANNER_STRING_VALUE;
                } else if (input == '"') {
                    state = SCANNER_STRING_FINAL;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_STRING_VALUE:
                if (input > 31 && input != '"') {
                    pToken->str[str_index++] = input;
                } else if (input == '\\') {
                    pToken->str[str_index++] = input;
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
                pToken->characterNumber = characterCount;
                pToken->lineNumber = lineCount;
                str_index = 0;
                ungetc(input, inFile);
                state = SUCCESS;
                return state;
            case SCANNER_ESC_SEQ:
                if (input == '0') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_0;
                } else if (input == '1') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_1;
                } else if (input == '2') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_2;
                } else if (input == 't' || input == 'n' || input == '"' || input == '/') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_STRING_VALUE;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_0:
                if (input == '0') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_00;
                } else if (input > '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_DEF;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_1:
                if (input >= '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_DEF;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_2:
                if (input == '5') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_25;
                } else if (input >= '0' && input <= '4') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_SEQ_DEF;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_00:
                if (input >= '1' && input <= '9') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_STRING_VALUE;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_DEF:
                if (input >= '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_STRING_VALUE;
                } else {
                    state = SCANNER_ERROR;
                }
                characterCount++;
                break;
            case SCANNER_SEQ_25:
                if (input >= '0' && input <= '5') {
                    pToken->str[str_index++] = input;
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
                    pToken->str[0] = '-';
                    pToken->lineNumber = lineCount;
                    pToken->characterNumber = characterCount;
                    state = SUCCESS;
                    return state;
                }
                characterCount++;
                break;
            case SCANNER_PLUS:
                ungetc(input, inFile);
                pToken->str[0] = '+';
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;
            case SCANNER_DIVIDE:
                if (input == '/') {
                    pToken->str[0] = '/';
                    state = SCANNER_INT_DIVIDE;
                    break;
                } else {
                    ungetc(input, inFile);
                    pToken->str[0] = '/';
                    pToken->lineNumber = lineCount;
                    pToken->characterNumber = characterCount;
                    state = SUCCESS;
                    return state;
                }
            case SCANNER_INT_DIVIDE:
                ungetc(input, inFile);
                pToken->str[1] = '/';
                pToken->type = TOKEN_INT_DIVIDE;
                pToken->lineNumber = lineCount;
                pToken->characterNumber = ++characterCount;
                state = SUCCESS;
                return state;
            case SCANNER_MULTIPLY:
                ungetc(input, inFile);
                pToken->str[0] = '*';
                pToken->characterNumber = characterCount;
                pToken->lineNumber = lineCount;
                state = SUCCESS;
                return state;
            case SCANNER_DOT:
                if (input == '.') {
                    pToken->str[0] = '.';
                    state = SCANNER_CONCATENATE;
                    characterCount++;
                    pToken->type = TOKEN_CONCATENATE;
                } else {
                    state = SCANNER_ERROR;
                    ungetc(input, inFile);
                }
                break;
            case SCANNER_CONCATENATE:
                ungetc(input, inFile);
                pToken->str[1] = '.';
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;
            case SCANNER_COLON:
                ungetc(input, inFile);
                pToken->str[0] = ':';
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;

            case SCANNER_ONE_EQUALS:
                if (input == '=') {
                    state = SCANNER_EQ;
                    pToken->type = TOKEN_EQUALS;
                    pToken->str[0] = '=';
                    characterCount++;
                } else {
                    state = SCANNER_ASSIGN;
                    pToken->type = TOKEN_ASSIGN;
                    pToken->str[0] = '=';
                    ungetc(input, inFile);
                }
                break;
            case SCANNER_EQ:
                ungetc(input, inFile);
                pToken->str[1] = '=';
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;

            case SCANNER_LT:
                if (input == '=') {
                    characterCount++;
                    state = SCANNER_LEQ;
                    break;
                } else {
                    ungetc(input, inFile);
                    pToken->str[0] = '<';
                    pToken->lineNumber = lineCount;
                    pToken->characterNumber = characterCount;
                    state = SUCCESS;
                    return state;
                }
            case SCANNER_GT:
                if (input == '=') {
                    state = SCANNER_GTE;
                    pToken->characterNumber = ++characterCount;
                    break;
                } else {
                    ungetc(input, inFile);
                    pToken->str[0] = '>';
                    pToken->lineNumber = lineCount;
                    pToken->characterNumber = characterCount;
                    state = SUCCESS;
                    return state;
                }
            case SCANNER_LEQ:
                ungetc(input, inFile);
                pToken->str[1] = '=';
                pToken->type = TOKEN_LEQ;
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;
            case SCANNER_GTE:
                ungetc(input, inFile);
                pToken->str[1] = '=';
                pToken->type = TOKEN_GEQ;
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;
            case SCANNER_TILDA:
                if (input == '=') {
                    pToken->str[0] = '~';
                    pToken->type = TOKEN_NEQ;
                    state = SCANNER_NEQ;
                } else {
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_NEQ:
                ungetc(input, inFile);
                pToken->str[1] = '=';
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;

            case SCANNER_GET_LENGTH:
                ungetc(input, inFile);
                pToken->str[1] = '#';
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                state = SUCCESS;
                return state;

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
                state = SCANNER_START;
                break;

            case SCANNER_INT:
                if (input >= '0' && input <= '9') {

                } else if (input == '.') {
                    state = SCANNER_NUMBER_POINT;
                } else {
                    ungetc(input, inFile);
                    pToken->characterNumber = ++characterCount;
                    pToken->lineNumber = lineCount;
                    pToken->type = TOKEN_INTEGER_LIT;
                    state = SUCCESS;
                    return state;
                }
                pToken->str[str_index++] = input;
                characterCount++;
                break;

            case SCANNER_NUMBER_POINT:
                if (input == 'e' || input == 'E') {
                    pToken->str[str_index++] = input;
                    characterCount++;
                    state = SCANNER_EXP_BASE;
                } else if (input >= '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_DOUBLE;
                    characterCount++;
                } else {
                    ungetc(input, inFile);
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_DOUBLE:
                if (input >= '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    characterCount++;
                } else if (input == 'e' || input == 'E') {
                    pToken->str[str_index++] = input;
                    characterCount++;
                    state = SCANNER_EXP_BASE;
                } else {
                    ungetc(input, inFile);
                    pToken->characterNumber = ++characterCount;
                    pToken->lineNumber = lineCount;
                    pToken->type = TOKEN_DOUBLE_LIT;
                    state = SUCCESS;
                    return state;
                }
                break;
            case SCANNER_EXP_BASE:
                if (input >= '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    characterCount++;
                    state = SCANNER_EXP;
                } else if (input == '+' || input == '-') {
                    pToken->str[str_index++] = input;
                    state = SCANNER_EXP_SIGN;
                } else {
                    ungetc(input, inFile);
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_EXP_SIGN:
                if (input >= '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    characterCount++;
                    state = SCANNER_EXP;
                } else {
                    ungetc(input, inFile);
                    state = SCANNER_ERROR;
                }
                break;
            case SCANNER_EXP:
                if (input >= '0' && input <= '9') {
                    pToken->str[str_index++] = input;
                    characterCount++;
                } else {
                    ungetc(input, inFile);
                    pToken->characterNumber = ++characterCount;
                    pToken->lineNumber = lineCount;
                    pToken->type = TOKEN_DOUBLE_LIT;
                    state = SUCCESS;
                    return state;
                }
                break;

            case SCANNER_IDENTIFIER:
                ungetc(input, inFile);
                characterCount += handleIdentifier(pToken, input);
                printf("%s\n", pToken->str);
                pToken->lineNumber = lineCount;
                state = SUCCESS;
                return state;

            case SCANNER_EOF:
                return state;


            default:
            case SCANNER_ERROR:
                pToken->characterNumber = characterCount;
                pToken->lineNumber = lineCount;
                str_index = 0;
                ungetc(input, inFile);
                state = ERR_LEXICAL;
                return state;
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

