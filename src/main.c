/**
 * @file main.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "main.h"

static const char *errorStrings[] = {
        [SUCCESS] = "", // never happens
        [ERR_LEXICAL] = "Lexical",
        [ERR_SYNTAX] = "Syntax",
        [ERR_SEMANTIC_DEF] = "Semantic - undefined variable / function, or attempted function / variable redefinition (DEF)",
        [ERR_SEMANTIC_ASSIGN] = "Semantic - type incompatibility (ASSIGN)",
        [ERR_SEMANTIC_FUNC] = "Semantic - wrong number / type of parameters / returns in function call (FUNC)",
        [ERR_SEMANTIC_EXPR] = "Semantic - type incompatibility in arithmetic / string / relation expressions (EXPR)",
        [ERR_SEMANTIC_OTHER] = "Semantic - other (OTHER)",
        [ERR_RUNTIME_NIL] = "Runtime",
        [ERR_RUNTIME_DIV_ZERO] = "Runtime",
        [ERR_INTERNAL] = "Internal",
};

int main() {
    scanner_init(stdin);

    Status s = parser_run();
    if (s == SUCCESS) {
        fprintf(stderr, "Compilation was successful!\n");
    } else {
        fprintf(stderr, "Compilation failed with %s error\n", errorStrings[s]);
    }
    scanner_destroy();
    return s;
}
