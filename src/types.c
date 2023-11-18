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

// Just for convenience
bool isNumeric(Type type) {
    return (type == INTEGER) || (type == NUMBER);
}

// Used to convert token type of literals to type
Type token_literal_to_type(TokenType tt) {
    switch (tt) {
        case TYPE_INTEGER_LIT:
            return INTEGER;

        case TYPE_DOUBLE_LIT:
            return NUMBER;

        case TYPE_STRING_LIT:
            return STRING;

        case TYPE_NIL:
            return NIL;

        default:
            return TYPE_NONE;
    }
}

// Used to convert token type of keywords to type
Type token_keyword_to_type(TokenType tt) {
    switch (tt) {
        case TYPE_INTEGER_KW:
            return INTEGER;

        case TYPE_NUMBER_KW:
            return NUMBER;

        case TYPE_STRING_KW:
            return STRING;

        case TYPE_NIL:
            return NIL;

        default:
            return TYPE_NONE;
    }
}

// List constructor
TypeList list_init() {
    TypeList list;
    list.first = NULL;
    list.active = NULL;
    return list;
}

// Append an item behind an existing list
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

// Set activity to the first item
void list_first(TypeList *list) {
    list->active = list->first;
}

// Set activity to the next item
void list_next(TypeList *list) {
    if (list->active) {
        list->active = list->active->next;
    }
}

// Return whether a list is active or not
bool list_is_active(TypeList *list) {
    return list->active != NULL;
}

// Return type of the active item
Type list_get_active(TypeList *list) {
    if (list->active) {
        return list->active->type;
    }
    return TYPE_NONE;
}

// List destructor
void list_destroy(TypeList *list) {
    TypeListItem *current = list->first;
    while (current) {
        TypeListItem *deleted = current;
        current = current->next;
        free(deleted);
    }
}

// Return index of currently active item
unsigned list_active_index(TypeList *list) {
    unsigned count = 0;
    TypeListItem *current = list->first;
    while ((current != list->active) && (current != NULL)) {
        current = current->next;
        count++;
    }
    return count;
}

// Return number of list items
unsigned list_count(TypeList *list) {
    unsigned i = 0;
    TypeListItem *current = list->first;
    while (current != NULL) {
        current = current->next;
        i++;
    }
    return i;
}

// Check if two srcList can be assigned / converted to dstList
bool list_can_assign(TypeList *dstList, TypeList *srcList) {
    list_first(dstList);
    list_first(srcList);
    while (list_is_active(dstList) && list_is_active(srcList)) {
        Type dstType = list_get_active(dstList);
        Type srcType = list_get_active(srcList);
        if (!can_assign(dstType, srcType)){
            return false;
        }
        list_next(dstList);
        list_next(srcList);

    }
    return true;
}

// Check if src can be assigned / converted to dst without any incompability
bool can_assign(Type dst, Type src) {
    if (dst == src) {
        return true;
    }
    if (dst == NUMBER && src == INTEGER) {
        return true;
    }
    if ((src == NIL) && (dst != TYPE_NONE)) {
        return true;
    }
    return false;
}
