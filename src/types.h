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

typedef enum {
    TYPE_NONE,
    NIL,
    INTEGER,
    NUMBER,
    STRING
} Type;

typedef struct typelistitem TypeListItem;

struct typelistitem {
    Type type;
    TypeListItem *next;
};

typedef struct {
    TypeListItem *first;
    TypeListItem *active;
} TypeList;

void list_init(TypeList *list);
Status list_append(TypeList *list, Type type);
void list_first(TypeList *list);
void list_next(TypeList *list);
bool list_is_active(TypeList *list);
Type list_get_active(TypeList *list);
void list_destroy(TypeList *list);

#endif //TYPES_H
