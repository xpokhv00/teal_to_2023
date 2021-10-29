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
#include "scanner.h"

// The file, that is being read
static FILE *inFile;

// Count lines from the start of file and store it in tokens
// this is for easier debugging
static unsigned lineCount;
// characters form the start of the line
static unsigned characterCount;

// if a letter is read after the token has ended, it is kept here
// until the next reading
static int nextLetter;

int handleIdentifier(Token *pToken, char input) {
    int i = 1;
    while (true) {
        input = getc(inFile);

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

    pToken->str[i] = '\0';
    if(!strcmp(pToken->str, "do")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_DO; return i;}
    else if(!strcmp(pToken->str, "else")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_ELSE; return i;}
    else if(!strcmp(pToken->str, "end")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_END; return i;}
    else if(!strcmp(pToken->str, "function")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_FUNCTION; return i;}
    else if(!strcmp(pToken->str, "global")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_GLOBAL; return i;}
    else if(!strcmp(pToken->str, "if")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_IF; return i;}
    else if(!strcmp(pToken->str, "integer")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_INTEGER; return i;}
    else if(!strcmp(pToken->str, "local")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_LOCAL; return i;}
    else if(!strcmp(pToken->str, "nil")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_NIL; return i;}
    else if(!strcmp(pToken->str, "number")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_NUMBER; return i;}
    else if(!strcmp(pToken->str, "require")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_REQUIRE; return i;}
    else if(!strcmp(pToken->str, "return")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_RETURN; return i;}
    else if(!strcmp(pToken->str, "string")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_STRING; return i;}
    else if(!strcmp(pToken->str, "then")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_THEN; return i;}
    else if(!strcmp(pToken->str, "while")){pToken->type = TOKEN_KEYWORD; pToken->keyword = KEYWORD_WHILE; return i;}
    else{pToken->type = TOKEN_IDENTIFIER;return i;}
}


int handleNumbers(Token *pToken, char input) { // TODO input must be int. Never ever use char for getc
    int i = 1;
    input = getc(inFile);
    // Int
    while (true) {

        if (isspace(input)) {
            pToken->type = TOKEN_INT;
            return i;
        } else if (input >= 48 && input <= 57) {
            pToken->str[i++] = input;
            pToken->str[i] = '\0';
        } else if (input == '.') {
            pToken->str[i++] = input;
            pToken->type = TOKEN_DOUBLE;
            break;
        } else {
            return -2;
        }
        input = getc(inFile);
    }
    input = getc(inFile);
    int e = i;
    // Double
    while (true) {
        if (input >= 48 && input <= 57) {
            pToken->str[i++] = input;
        } else if (input == 'e' || input == 'E') {
            pToken->type = TOKEN_EXP;
            pToken->str[i++] = input;
            break;

        } else if (isspace(input)) {
            if (e == i) {
                return -2;
            } else {
                return i;
            }
        } else {
            return -2;
        }

        input = getc(inFile);

    }
    // Exp
    int z = i;
    input = getc(inFile);
    while (true) {

        if ((input == '+' || input == '-') && z == i) {
            pToken->str[i++] = input;

        } else if (input >= 48 && input <= 57) {
            pToken->str[i++] = input;
        } else if (isspace(input) && z != i) {
            return i;
        } else {
            return -2;
        }

        input = getc(inFile);
    }
    return i;
}

int handleEscapeSequence(char input, int i, Token *pToken) {
    // 0
    if (input == 48) {
        pToken->str[i] = input;
        input = getc(inFile);
        i++;
        if (input == 48) {
            pToken->str[i] = input;
            input = getc(inFile);
            if (input == 48 || input > 57) {
                return -2;
            }
            pToken->str[++i] = input;
            return i;
        } else if (input <= 57) {
            pToken->str[i] = input;
            input = getc(inFile);
            if (input >= 48) {
                pToken->str[++i] = input;
                return i;
            } else {
                return -2;
            }
        }
    }
        // 1
    else if (input == 49) {
        pToken->str[i] = input;
        input = getc(inFile);
        if (input >= 48 && input <= 57) {
            pToken->str[++i] = input;
            input = getc(inFile);
            if (input >= 48 && input <= 57) {
                pToken->str[++i] = input;
                return i;
            } else {
                return -2;
            }
        }

    }
        // 2
    else if (input == 50) {
        pToken->str[i] = input;
        input = getc(inFile);
        if (input == 53) {
            pToken->str[++i] = input;
            input = getc(inFile);
            if (input <= 53 && input >= 48) {
                pToken->str[++i] = input;
                return i;
            } else {
                return -2;
            }
        } else if (input >= 48 && input <= 52) {
            pToken->str[++i] = input;
            input = getc(inFile);
            if (input >= 48 && input <= 57) {
                pToken->str[++i] = input;
                return i;
            }
        } else {
            return -2;
        }
    } else {
        return -2;
    }
    return i;
}

Status scanner_init(FILE *in) {
    inFile = in;
    return SUCCESS;
}

Status scanner_get_token(Token *pToken) {

    memset(pToken->str,0,strlen(pToken->str));
    if (inFile == NULL) {
        return ERR_INTERNAL;
    }

    int state = SCANNER_START;
    char input; // TODO it must be int, because EOF is not in range of char

    while (true) {
        input = getc(inFile);
        characterCount++;

        switch (state) { // TODO add default, that will end with error
            case SCANNER_START:
                if (input == '\n') {
                    lineCount++;
                    state = SCANNER_EOL;

                } else if (input == ':') {
                    ungetc(input, inFile);
                    state = SCANNER_SINGLE_OPERATOR;
                    pToken->type = TOKEN_DVOJTECKAASI;

                } else if (input == '+') {
                    ungetc(input, inFile);
                    state = SCANNER_SINGLE_OPERATOR;
                    pToken->type = TOKEN_PLUS;

                } else if (input == '-') {
                    input = getc(inFile);
                    if (input == '-') {
                        characterCount++;
                        state = SCANNER_COMMENTARY;
                    } else {
                        ungetc(input, inFile);
                        state = SCANNER_SINGLE_OPERATOR;
                        pToken->type = TOKEN_MINUS;
                    }

                } else if (input == '*') {
                    ungetc(input, inFile);
                    state = SCANNER_SINGLE_OPERATOR;
                    pToken->type = TOKEN_MULTIPLY;

                } else if (input == '/') {
                    input = getc(inFile);
                    if (input == '/') {
                        pToken->type = TOKEN_INT_DIVIDE;
                        characterCount++;
                        pToken->lineNumber = lineCount;
                        pToken->characterNumber = characterCount;
                        pToken->str = "//";
                        pToken->str[2] = '\0';
                        state = SCANNER_END;

                    } else {
                        ungetc(input, inFile);
                        state = SCANNER_SINGLE_OPERATOR;
                        pToken->type = TOKEN_DIVIDE;

                    }
                } else if (input == '.') {
                    state = SCANNER_SINGLE_OPERATOR;
                    input = getc(inFile);
                    if (input == '.') {
                        pToken->type = TOKEN_CONCATENATE;
                        characterCount++;
                        pToken->lineNumber = lineCount;
                        pToken->characterNumber = characterCount;
                        pToken->str = "..";
                        pToken->str[2] = '\0';
                        state = SCANNER_END;

                    } else {
                        state = SCANNER_ERROR;
                        ungetc(input, inFile);

                    }
                } else if (input == '>') {
                    pToken->str[0] = input;
                    nextLetter = input;
                    state = SCANNER_DOUBLE_OPERATOR;
                    pToken->type = TOKEN_GT;

                } else if (input == '<') {
                    pToken->str[0] = input;
                    nextLetter = input;
                    state = SCANNER_DOUBLE_OPERATOR;
                    pToken->type = TOKEN_LT;

                } else if (input == '~') {
                    pToken->str[0] = input;
                    nextLetter = input;
                    state = SCANNER_DOUBLE_OPERATOR;
                    pToken->type = TOKEN_NEQ;

                } else if (input == '=') {
                    pToken->str[0] = input;
                    nextLetter = input;
                    state = SCANNER_DOUBLE_OPERATOR;
                    pToken->type = TOKEN_ASSIGN;

                } else if (input == '#') {
                    state = SCANNER_SINGLE_OPERATOR;
                    pToken->type = TOKEN_GET_LENGTH;

                } else if (input == '\"') {
                    state = SCANNER_STRING;
                    pToken->type = TOKEN_STRING;
                } else if (isspace(input)) {
                    continue;
                } else if (input >= 48 && input <= 57) {
                    pToken->str[0] = input;
                    characterCount++;
                    int i = handleNumbers(pToken, input);
                    if (i == -2) {
                        state = SCANNER_ERROR;
                    } else {
                        state = SCANNER_END;
                    }

                } else if ((input >= 65 && input <= 90) || (input >= 97 && input <= 122) || input == '_') {
                    pToken->str[0] = input;
                    int g = handleIdentifier(pToken, input);
                    characterCount += g;
                    state = SCANNER_END;

                }
                break;

            case SCANNER_STRING:
                for (int i = 0; true; ++i) {

                    if (input > 31 && input != '"') {
                        pToken->str[i] = input;
                    } else if (input == '"') {
                        pToken->str[i] = '"';
                        break;
                    } else if (input == '\\') {
                        pToken->str[i] = '\\';
                        i++;
                        input = getc(inFile);
                        if (input == 't') {
                            pToken->str[i] = '\t';
                        } else if (input == 'n') {
                            pToken->str[i] = '\n';
                        } else if (input == '"') {
                            pToken->str[i] = '"';
                        } else if (input == '\\') {
                            pToken->str[i] = '\\';
                        } else if (input >= 48 && input <= 50) {
                            int e = handleEscapeSequence(input, i, pToken);
                            if (e == -2) {
                                state = SCANNER_ERROR;
                                break;
                            }
                            // (pocet prectenych znaku ve fukci) - (pocet doted prectenych znaku)
                            characterCount += e - i;
                            i = e;
                        } else {
                            state = SCANNER_ERROR;
                            break;
                        }

                    }
                    input = getc(inFile);
                    characterCount++;
                }

            case SCANNER_COMMENTARY:
                if (input == '[') {
                    input = getc(inFile);
                    characterCount++;
                    pToken->type = TOKEN_COMMENT;
                    if (input == '[') {
                        state = SCANNER_BLOCK_COMMENTARY;
                    } else {
                        state = SCANNER_LINE_COMMENTARY;
                        pToken->str[0] = '[';
                        ungetc(input, inFile);
                        characterCount--;
                    }
                } else if (input == '\n') {
                    state = SCANNER_END;
                    pToken->type = TOKEN_COMMENT;
                }else{
                    ungetc(input, inFile);
                    state = SCANNER_LINE_COMMENTARY;
                    pToken->type = TOKEN_COMMENT;
                }
                break;

            case SCANNER_BLOCK_COMMENTARY:

                for (int i = 0; true; ++i) {

                    if (input == ']') {
                        input = getc(inFile);
                        characterCount++;

                        if (input == ']') {
                            pToken->type = TOKEN_COMMENT;
                            state = SCANNER_END;
                            break;
                        } else if (input == '\n') {
                            pToken->type = TOKEN_COMMENT;
                            state = SCANNER_END;
                        } else {
                            i++;
                            pToken->str[i] = input;
                            ungetc(input, inFile);
                            characterCount--;
                        }
                    }

                    if (input == EOF) {
                        state = SCANNER_ERROR;
                        break;
                    }
                    pToken->str[i] = input;
                    input = getc(inFile);
                    characterCount++;
                }

                break;

            case SCANNER_LINE_COMMENTARY:

                for (int i = 0; input != '\n'; ++i) {

                    pToken->str[i] = input;
                    input = getc(inFile);
                    characterCount++;
                }
                pToken->type = TOKEN_COMMENT;
                state = SCANNER_END;
                break;

            case SCANNER_DOUBLE_OPERATOR:
                characterCount++;
                switch (pToken->type) {
                    case TOKEN_ASSIGN:
                        if (input == '=') {
                            pToken->type = TOKEN_EQUALS;
                            pToken->str[1] = '=';
                            pToken->str[2] = '\0';
                            pToken->characterNumber = characterCount;
                            pToken->lineNumber = lineCount;
                            pToken->type = TOKEN_EQUALS;
                        } else {
                            ungetc(input, inFile);

                            characterCount--;
                        }
                        state = SCANNER_END;
                        break;

                    case TOKEN_NEQ:
                        if (input == '=') {
                            pToken->str[1] = '=';
                            pToken->str[2] = '\0';
                            pToken->lineNumber = lineCount;
                            pToken->characterNumber = characterCount;
                            state = SCANNER_END;
                        } else {
                            pToken->type = TOKEN_ERROR;
                            state = SCANNER_ERROR;
                            ungetc(input, inFile);
                            characterCount--;
                        }
                        break;
                    case TOKEN_LT:
                        if (input == '=') {
                            pToken->type = TOKEN_LEQ;
                            pToken->str[1] = '=';
                            pToken->str[2] = '\0';
                            pToken->lineNumber = lineCount;
                            pToken->characterNumber = characterCount;
                            state = SCANNER_END;
                        } else {
                            ungetc(input, inFile);
                            characterCount--;
                            input = '>';
                            state = SCANNER_SINGLE_OPERATOR;
                        }
                        break;

                    case TOKEN_GT:
                        if (input == '=') {
                            pToken->type = TOKEN_GEQ;
                            pToken->str[1] = '=';
                            pToken->str[2] = '\0';
                            pToken->lineNumber = lineCount;
                            pToken->characterNumber = characterCount;
                            state = SCANNER_END;
                        } else {
                            ungetc(input, inFile);
                            characterCount--;
                            input = '<';
                            state = SCANNER_SINGLE_OPERATOR;
                        }
                        break;
                }
                break;

            case SCANNER_EOL:
                pToken->characterNumber = characterCount;
                pToken->lineNumber = lineCount;
                pToken->type = TOKEN_EOL;
                lineCount += 1;
                characterCount = 0;
                break;


            case SCANNER_SINGLE_OPERATOR:
                pToken->str[0] = input;
                pToken->str[1] = '\0';
                pToken->lineNumber = lineCount;
                pToken->characterNumber = characterCount;
                return state;
                break;

            case SCANNER_END:
                ungetc(input, inFile);
                return state;

            case SCANNER_ERROR:
                ungetc(input, inFile);
                return state;
        }

        if (input == ' ') {
            break;
        }
    }
}

Status scanner_destroy() {
    // deallocate all the resources
    fclose(inFile);
    return SUCCESS;
}

