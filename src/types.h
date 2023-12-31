/**
 * @file types.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

#ifndef TYPES_H
#define TYPES_H

#include "common.h"
#include "stdlib.h"
#include "scanner.h"

typedef enum {
    TYPE_NONE,
    NIL,
    INTEGER,
    NUMBER,
    STRING,
    BOOL
} Type;

typedef struct typelistitem TypeListItem;

struct typelistitem {
    Type type;              // Data type of item
    TypeListItem *next;     // Pointer to the next item
};

typedef struct {
    TypeListItem *first;    // Pointer to the first item
    TypeListItem *active;   // Pointer to the active item
} TypeList;

// Token and type functions
bool isNumeric(Type type);
Type token_keyword_to_type(TokenType tt);
Type token_literal_to_type(TokenType tt);
bool can_assign(Type dst, Type src);

// List functions
TypeList list_init();
Status list_append(TypeList *list, Type type);
void list_first(TypeList *list);
void list_next(TypeList *list);
bool list_is_active(TypeList *list);
Type list_get_active(TypeList *list);
void list_destroy(TypeList *list);
unsigned list_active_index(TypeList *list);
unsigned list_count(TypeList *list);
bool list_can_assign(TypeList *dstList, TypeList *srcList);

#endif //TYPES_H
