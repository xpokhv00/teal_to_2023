/**
 * @file generator.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "generator.h"

#define BUFFER_MIN_FREE 1024

typedef struct {
    int bufferLevels;
    char *buffer;
    size_t bufferSize;
    FILE *out;
    int labelCounter;
} Printer;

static Printer printer;

Status gen_init(FILE *outFile) {
    printer.out = outFile;
    printer.bufferLevels = 0;
    printer.buffer = malloc(BUFFER_MIN_FREE);
    if (printer.buffer == NULL) {
        return ERR_INTERNAL;
    }
    printer.buffer[0] = '\0'; // empty string
    printer.bufferSize = BUFFER_MIN_FREE;

    printer.labelCounter = 0;
    return SUCCESS;
}

void gen_buffer_start() {
    printer.bufferLevels++;
}

void gen_buffer_stop() {
    printer.bufferLevels--;
    if (printer.bufferLevels == 0) {
        fprintf(printer.out, "%s", printer.buffer);
        printer.buffer[0] = '\0'; // empty string
    }
}

Status gen_print(const char *format, ...) {
    va_list args;
    va_start(args, format);

    if (printer.bufferLevels > 0) {
        // reallocate if not enough space
        size_t n = printer.bufferSize - strlen(printer.buffer);
        if (n < BUFFER_MIN_FREE) {
            printer.bufferSize *= 2;
            char *tmp = realloc(printer.buffer, printer.bufferSize);
            if (tmp == NULL) {
                return ERR_INTERNAL;
            }
            printer.buffer = tmp;
        }
        char *end = printer.buffer + strlen(printer.buffer);
        vsnprintf(end, BUFFER_MIN_FREE, format, args);
    } else {
        vfprintf(printer.out, format, args);
    }

    va_end(args);
    return SUCCESS;
}

Status gen_prepend(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(printer.out, format, args);
    va_end(args);
    return SUCCESS;
}

void gen_destroy() {
    // there should be nothing left in the buffer
    free(printer.buffer);
}

Status gen_print_literal(Token token) {
    switch (token.type) {
        case TOKEN_INTEGER_LIT:
            gen_print("int@%s", token.str);
            break;

        case TOKEN_DOUBLE_LIT:;
            double value = strtod(token.str, NULL);
            gen_print("float@%a", value);
            break;

        case TOKEN_STRING_LIT:;
            // string with actual byte values, escape sequences will be decoded
            char *in = token.str;
            char *out = calloc(4*strlen(token.str)+10, sizeof(char));
            if (out == NULL) {
                return ERR_INTERNAL;
            }

            int i = 1; // input index
            while ((in[i] != '\0') && (in[i] != '\"')) {
                char character = in[i];

                if (in[i] == '\\') {
                    i++;
                    switch (in[i]) {
                        case '\"':
                        case '\\':
                            character = in[i];
                            break;

                        case 'n':
                            character = '\n';
                            break;

                        case 't':
                            character = '\t';
                            break;

                        default:
                            character = atoi(&in[i]);
                            i += 2;
                            break;
                    }
                }
                sprintf(out + strlen(out),"\\%.3d", (int)character);
                i++;
            }

            gen_print("string@%s", out);
            free(out);
            break;

        case TOKEN_NIL:
            gen_print("nil@nil");
            break;

        default:
            return ERR_INTERNAL;
    }
    return SUCCESS;
}


Status gen_print_value(Token token, SymTab* st) {
    if (token.type == TOKEN_IDENTIFIER) {
        return gen_print_var(token, st);
    }
    return gen_print_literal(token);
}

Status gen_print_var(Token token, SymTab* st) {
    if (token.type == TOKEN_IDENTIFIER) {
        HTabPair *pair = st_lookup(st, token.str);
        if (pair == NULL) {
            return ERR_INTERNAL;
        }
        gen_print("LF@$%u", pair->value.ID);
    }
    return SUCCESS;
}

Status gen_write(Token token, SymTab *st) {
    int nilLabel = gen_new_label();
    int endLabel = gen_new_label();

    // Move the value to print into register a
    gen_print("MOVE ");
    gen_print(" GF@a ");
    gen_print_value(token, st);
    gen_print("\n");
    // Save its type into register b
    gen_print("TYPE GF@b GF@a\n");
    // if there is nil, jump to nil label
    gen_print("JUMPIFEQ %%%d GF@b string@nil\n", nilLabel);

    // otherwise, use write instruction
    gen_print("WRITE GF@a\n");
    gen_print("JUMP %%%d\n", endLabel);

    // print nil
    gen_print("LABEL %%%d\n", nilLabel);
    gen_print("WRITE string@nil\n");

    // end
    gen_print("LABEL %%%d\n", endLabel);
    return SUCCESS;
}

int gen_new_label() {
    return printer.labelCounter++;
}

Status gen_conditional(int trueLabel, int falseLabel) {
    int boolLabel = gen_new_label();

    // expression result will be on the top of the stack
    // pop it into a
    gen_print("POPS GF@a\n");
    // save condition's type into b
    gen_print("TYPE GF@b GF@a\n");
    // if the expression is bool, jump to its label
    gen_print("JUMPIFEQ %%%d GF@b string@bool\n", boolLabel);

    // evaluate anything other than bool
    // expression is false, if the type is nil
    gen_print("JUMPIFEQ %%%d GF@b string@nil\n", falseLabel);
    // otherwise, it's true
    gen_print("JUMP %%%d\n", trueLabel);

    // evaluate bool
    gen_print("LABEL %%%d\n", boolLabel);
    // if expression is false, jump to else label
    gen_print("JUMPIFEQ %%%d GF@a bool@false\n", falseLabel);
    // if it is true, jump to true label
    gen_print("JUMP %%%d\n", trueLabel);

    return SUCCESS;
}

