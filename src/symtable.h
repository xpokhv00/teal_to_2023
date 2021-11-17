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

#define SYMTAB_SIZE 32

// Tabulka:
struct symtab;                        // neúplná deklarace struktury - uživatel nevidí obsah
typedef struct symtab SymTab;         // typedef podle zadání

// Typy:
typedef const char *SymTabKey;
typedef struct {
    bool        defined;
    TypeList    paramList;          // Only used in functions
    TypeList    returnList;         // Only used in functions
    Type        varType;            // Only used in variables
} SymTabValue;

// Dvojice dat v tabulce
typedef struct  {
    SymTabKey    key;              // nazev funkce/promenne
    SymTabValue  value;            // struktura se seznamem parametru
} SymTabPair;                      // typedef podle zadání

// Rozptylovací (hash) funkce (stejná pro všechny tabulky v programu)
// Pokud si v programu definujete stejnou funkci, použije se ta vaše.
size_t symtab_hash_function(SymTabKey str);

// Funkce pro práci s tabulkou:
SymTab *symtab_init();                    // konstruktor tabulky
SymTab *symtab_move(size_t n, SymTab *from);      // přesun dat do nové tabulky
size_t symtab_size(const SymTab *table);              // počet záznamů v tabulce
size_t symtab_bucket_count(const SymTab *t);      // velikost pole

SymTabPair *symtab_find(SymTab *table, SymTabKey key);      // hledání
SymTabPair *symtab_insert(SymTab *table, SymTabKey key, SymTabValue value);

bool symtab_erase(SymTab *table, SymTabKey key);    // ruší zadaný záznam

// for_each: projde všechny záznamy a zavolá na ně funkci f
void symtab_for_each(const SymTab *table, void (*f)(SymTabPair *data));

void symtab_clear(SymTab *table);    // ruší všechny záznamy
void symtab_destroy(SymTab *table);     // destruktor tabulky

#endif // SYMTABLE_H
