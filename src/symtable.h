/**
 * @file symtable.h
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
typedef struct symboltable SymbolTable;

// Types:
typedef const char *HTabKey;
typedef struct {
    bool        defined;
    TypeList    paramList;          // Only used in functions
    TypeList    returnList;         // Only used in functions
    Type        varType;            // Only used in variables
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
    bool readHigher;
};

struct symboltable {
    HTab *top;
};

// Hash table functions
HTab *htab_init();
size_t htab_hash_function(HTabKey str);
HTabPair *htab_find(HTab *table, HTabKey key);
HTabPair *htab_insert(HTab *table, HTabKey key, HTabValue value);
void htab_clear(HTab *table);
void htab_destroy(HTab *table);

// Symtable functions (table of symbols)
Status st_init(SymbolTable *st);
Status st_push_frame(SymbolTable *st, bool transparent);
HTabPair *st_lookup(SymbolTable *st, char *key);
void st_pop_frame(SymbolTable *st);
HTabPair *st_add(SymbolTable *st, char *key);
void st_destroy(SymbolTable *st);
Type st_token_to_type(SymbolTable *st, Token token);

#endif // SYMTABLE_H
