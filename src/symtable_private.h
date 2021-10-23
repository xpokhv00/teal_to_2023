/**
 * @file symtable_private.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

// The source of  this file is IJC project 2 2020/2021 (xlanro00)

#ifndef SYMTABLE_PRIVATE_H
#define SYMTABLE_PRIVATE_H

#include <stdio.h>
#include <stdlib.h>

#include "symtable.h"

// Encapsulated header file

typedef unsigned int index_t;   // Type of the index

// Full structure of an item
struct htab_item {
    htab_pair_t htab_pair;
    struct htab_item *next;
};

typedef struct htab_item htab_item_t;

// Hash table full structure
struct htab {
    htab_item_t **htab_items;
    index_t arr_size;
    index_t size;
};

#endif // SYMTABLE_PRIVATE_H
