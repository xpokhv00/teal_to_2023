/**
 * @file include.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "include.h"

const char *includeCode = ".IFJcode21\n"
                          "JUMP %start0\n";

#define SET_ATRIBUTES()   pair->value.varType = TYPE_NONE; \
                       pair->value.ID = st->idCounter++;\
                       pair->value.defined = true;

Status add_builtin_fn(SymTab *st) {
    // Builtin function reads
    HTabPair *pair = htab_insert(st->top, "reads");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.returnList), STRING);

    // Builtin function readi
    pair = htab_insert(st->top, "readi");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.returnList), INTEGER);

    // Builtin function readn
    pair = htab_insert(st->top, "readn");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.returnList), NUMBER);

    // Builtin function write
    pair = htab_insert(st->top, "write");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    pair->value.specialFn = true;

    // Builtin function tointeger
    pair = htab_insert(st->top, "tointeger");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), NUMBER);
    list_append((&pair->value.returnList), INTEGER);

    // Builtin function substr
    pair = htab_insert(st->top, "substr");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), STRING);
    list_append((&pair->value.paramList), NUMBER);
    list_append((&pair->value.paramList), NUMBER);
    list_append((&pair->value.returnList), STRING);

    // Builtin function ord
    pair = htab_insert(st->top, "ord");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), STRING);
    list_append((&pair->value.paramList), INTEGER);
    list_append((&pair->value.returnList), INTEGER);

    // Builtin function chr
    pair = htab_insert(st->top, "chr");
    if (pair == NULL) {
        return ERR_INTERNAL;
    }
    SET_ATRIBUTES();
    list_append((&pair->value.paramList), INTEGER);
    list_append((&pair->value.returnList), STRING);

    return SUCCESS;
}

