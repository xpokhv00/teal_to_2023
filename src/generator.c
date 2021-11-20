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

Status gen_print_internal(bool prepend, const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (printer.buffering && !prepend) {
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
        vfprintf(stderr, format, args);
    }
    va_end(args);
    return SUCCESS;
}

Status gen_print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    Status ret = gen_print_internal(false, format, args);
    va_end(args);
    return ret;
}

Status gen_prepend(const char *format, ...) {
    va_list args;
    va_start(args, format);
    Status ret = gen_print_internal(true, format, args);
    va_end(args);
    return ret;
}

void gen_destroy() {
    gen_buffer_stop();
    free(printer.buffer);
}