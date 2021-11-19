/**
 * @file types.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#include "types.h"

Type tokentype_to_type(TokenType tt) {
    switch (tt) {
        case TOKEN_INTEGER_LIT:
            return INTEGER;

        case TOKEN_DOUBLE_LIT:
            return NUMBER;

        case TOKEN_STRING_LIT:
            return STRING;

        case TOKEN_NIL:
            return NIL;

        default:
            return TYPE_NONE;
    }
}

void list_init(TypeList *list) {
    list->first = NULL;
}

Status list_append(TypeList *list, Type type) {
    TypeListItem **current = &list->first;
    while (*current) {
        current = &(*current)->next;
    }
    (*current) = malloc(sizeof(TypeListItem));

    if (*current == NULL) {
        return ERR_INTERNAL;
    }
    (*current)->next = NULL;
    (*current)->type = type;

    return SUCCESS;
}

void list_first(TypeList *list) {
    list->active = list->first;
}

void list_next(TypeList *list) {
    if (list->active) {
        list->active = list->active->next;
    }
}

bool list_is_active(TypeList *list) {
    return (list->active);
}

Type list_get_active(TypeList *list) {
    if (list->active) {
        return list->active->type;
    }
    return TYPE_NONE;
}

void list_destroy(TypeList *list) {
    TypeListItem *current = list->first;
    while (current) {
        TypeListItem *deleted = current;
        current = current->next;
        free(deleted);
    }
}
