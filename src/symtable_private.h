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

// Full structure of an item
struct symtabitem {
    SymTabPair symtab_pair;
    struct symtabitem *next;
};

typedef struct symtabitem SymTabItem;

// Hash table full structure
struct symtab {
    SymTabItem **symtab_items;
    unsigned int arr_size;
};

#endif // SYMTABLE_PRIVATE_H
