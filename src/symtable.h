/**
 * @file symtab.h
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

// The source of this file is a modified version of IJC project 2 2020/2021 (xlanro00)

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <string.h>
#include <stdbool.h>
#include "types.h"

#define HTAB_SIZE 32

// Table:
struct htab;
typedef struct htabitem HTabItem;
typedef struct htab HTab;
typedef struct symtab SymTab;

// Types:
typedef const char *HTabKey;
typedef struct {
    bool        defined;
    TypeList    paramList;          // Only used in functions
    TypeList    returnList;         // Only used in functions
    Type        varType;            // Only used in variables, TYPE_NONE for functions
    int ID; // each entry in symtable gets its own unique ID
} HTabValue;

// Pair of values in hash table
typedef struct  {
    HTabKey    key;              // Function/Variable name
    HTabValue  value;            // Structure containing data
} HTabPair;

// Full structure of an item
struct htabitem {
    HTabPair htab_pair;
    struct htabitem *next;
};

// Hash table full structure
struct htab {
    HTabItem **htab_items;
    unsigned int arr_size;
    HTab *next;
};

struct symtab {
    HTab *top;
    int idCounter;
};

// Hash table functions
HTab *htab_init();
size_t htab_hash_function(HTabKey str);
HTabPair *htab_find(HTab *table, HTabKey key);
HTabPair *htab_insert(HTab *table, HTabKey key);
bool htab_check_fn_define(const HTab *t);
void htab_clear(HTab *table);
void htab_destroy(HTab *table);

// Symtable functions (table of symbols)
Status st_init(SymTab *st);
Status st_push_frame(SymTab *st);
HTabPair *st_lookup(SymTab *st, const char *key);
void st_pop_frame(SymTab *st);
Status st_add(SymTab *st, Token token, HTabPair **pair);
void st_destroy(SymTab *st);
Type st_token_to_type(SymTab *st, Token token);
Status st_check_fn_defined(SymTab *st);

#endif // SYMTABLE_H
