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

Type token_literal_to_type(TokenType tt) {
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

Type token_keyword_to_type(TokenType tt) {
    switch (tt) {
        case TOKEN_INTEGER_KW:
            return INTEGER;

        case TOKEN_NUMBER_KW:
            return NUMBER;

        case TOKEN_STRING_KW:
            return STRING;

        case TOKEN_NIL:
            return NIL;

        default:
            return TYPE_NONE;
    }
}

TypeList list_init() {
    TypeList list;
    list.first = NULL;
    list.active = NULL;
    return list;
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
    list->active = (*current);
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
    return list->active != NULL;
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

unsigned list_active_index(TypeList *list) {
    unsigned count = 0;
    TypeListItem *current = list->first;
    while ((current != list->active) && (current != NULL)) {
        current = current->next;
        count++;
    }
    return count;
}

unsigned list_count(TypeList *list) {
    unsigned i = 0;
    TypeListItem *current = list->first;
    while (current != NULL) {
        current = current->next;
        i++;
    }
    return i;
}

bool list_can_assign(TypeList *dstList, TypeList *srcList) {
    list_first(dstList);
    list_first(srcList);
    Type dstType = list_get_active(srcList);
    Type srcType = list_get_active(dstList);
    while (can_assign(dstType, srcType)) {
        list_next(dstList);
        list_next(srcList);
        dstType = list_get_active(srcList);
        srcType = list_get_active(dstList);
        if (!list_is_active(dstList) || !list_is_active(srcList)) {
            return true;
        }
    }
    return false;
}

bool can_assign(Type dst, Type src) {
    if (dst == src) {
        return true;
    }
    if (dst == NUMBER && src == INTEGER) {
        return true;
    }
    if (src == NIL) {
        return true;
    }
    return false;
}
