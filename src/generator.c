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
    bool buffering;
    char *buffer;
    size_t bufferSize;
    FILE *out;
} Printer;

static Printer printer;

Status gen_init(FILE *outFile) {
    printer.out = outFile;
    printer.buffering = false;
    printer.buffer = malloc(BUFFER_MIN_FREE);
    if (printer.buffer == NULL) {
        return ERR_INTERNAL;
    }
    printer.buffer[0] = '\0'; // empty string
    printer.bufferSize = BUFFER_MIN_FREE;
    return SUCCESS;
}

void gen_buffer_start() {
    printer.buffering = true;
}

void gen_buffer_stop() {
    fprintf(printer.out, "%s", printer.buffer);
    printer.buffering = false;
    printer.buffer[0] = '\0'; // empty string
}

Status gen_print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (printer.buffering) {
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
        vsnprintf(printer.buffer, BUFFER_MIN_FREE, format, args);
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
    gen_buffer_stop();
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
            // first, get rid of the quote symbols
            size_t neededLength = strlen(token.str) + 1;
            char* str = malloc(neededLength);
            strcpy(str, &token.str[1]);
            str[strlen(str) - 1] = '\0';
            // TODO more complex conversion?
            gen_print("string@%s", str);
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

