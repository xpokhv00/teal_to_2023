/**
 * @file symtab.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

// The source of  this file is IJC project 2 2020/2021 (xlanro00)

#include "symtable.h"

// Constructor, creates an empty table
HTab *htab_init() {
    HTab *table = malloc(sizeof(HTab));
    if (table == NULL) {
        return NULL;
    } else {
        table->arr_size = HTAB_SIZE;
        table->htab_items = malloc(sizeof(HTabItem *) * HTAB_SIZE);
        if (table->htab_items == NULL) {
            return NULL;
        }

        // Set the whole array to null
        for(size_t i = 0; i < HTAB_SIZE; i++) {
            table->htab_items[i] = NULL;
        }
    }
    table->next = NULL;
    return table;
}

// Hash function of a hash table, used to calculate where each item goes in the table
size_t htab_hash_function(HTabKey str) {
    unsigned h = 0; // 32 bits
    const unsigned char *p;
    for(p = (const unsigned char*)str; *p != '\0'; p++) {
        h = 65599*h + *p;
    }
    return h;
}

// Finds an element in a hash table, returns pointer to its pair
HTabPair *htab_find(HTab *table, HTabKey key) {
    unsigned key_hash = htab_hash_function(key) % table->arr_size;
    HTabItem *temp = table->htab_items[key_hash];

    while (temp) {
        if ((strcmp(temp->htab_pair.key, key)) == 0) {
            return &(temp->htab_pair);
        }
        temp = temp->next;
    }
    return NULL;
}

// Searches for an item, creates if not found
HTabPair *htab_insert(HTab *table, HTabKey key) {
    unsigned key_hash = htab_hash_function(key) % table->arr_size;
    HTabItem **element = &(table->htab_items[key_hash]);

    // Find last item of the list
    while (*element) {
        if (strcmp((*element)->htab_pair.key, key) == 0) {
            break;
        }

        // Move to next item of the list
        element = &(*element)->next;
    }

    *element = malloc(sizeof(HTabItem));
    if (*element == NULL) {
        return NULL;
    }

    (*element)->htab_pair.key = malloc(strlen(key) + 1);
    if ((*element)->htab_pair.key == NULL) {
        //allocation of item failed
        free(*element);
        return NULL;
    }

    // Initialize new item to the default values
    strcpy((char *)(*element)->htab_pair.key , key);
    TypeList paramList = list_init();
    TypeList returnList = list_init();
    (*element)->htab_pair.value.specialFn = false;
    (*element)->htab_pair.value.defined = false;
    (*element)->htab_pair.value.paramList = paramList;
    (*element)->htab_pair.value.returnList = returnList;
    (*element)->htab_pair.value.varType = TYPE_NONE;
    (*element)->next = NULL;

    //returning new item pair
    return (&(*element)->htab_pair);
}

// Checks if declared function has been defined
bool htab_check_fn_define(const HTab *t) {
    for (size_t i=0; i < t->arr_size; i++) {
        HTabItem *item = t->htab_items[i];

        while (item != NULL) {
            if (item->htab_pair.value.defined == false) {
                return false;
            }
            item = item->next;
        }
    }
    return true;
}

// Deletes all elements of hash table, an empty allocated table is left behind
void htab_clear(HTab *table) {
    HTabItem *temp_element;

    // Delete items for every item in array
    for (unsigned i = 0; i < table->arr_size; i++) {
        // Delete the first item of a list
        temp_element = table->htab_items[i];
        table->htab_items[i]= NULL;

        // Free every item of a following list, its key
        while (temp_element != NULL) {
            HTabItem *current_element = temp_element->next;
            // free each element
            free((char *)temp_element->htab_pair.key);
            list_destroy(&temp_element->htab_pair.value.returnList);
            list_destroy(&temp_element->htab_pair.value.paramList);
            free(temp_element);
            temp_element = current_element;
        }
    }
}

// Destructor of a hash table
void htab_destroy(HTab *table) {
    htab_clear(table);
    free(table->htab_items);
    table->arr_size = 0;
    free(table);
}

// Symbol table constructor
Status st_init(SymTab *st) {
    st->top = htab_init();
    if (st->top == NULL) {
        return ERR_INTERNAL;
    }
    st->idCounter = 0;
    return SUCCESS;
}

// Used when moving into function / if / while
Status st_push_frame(SymTab *st) {
    HTab *next = st->top;
    st->top= htab_init();
    if (st->top == NULL) {
        return ERR_INTERNAL;
    }
    st->top->next = next;
    return SUCCESS;
}

// Find an item with the same key in the current table
HTabPair *st_lookup(SymTab *st, const char *key, bool topFrameOnly) {
    HTab *currentTable = st->top;
    bool firstPass = true;

    do {
        bool shouldCheck = true;

        if (topFrameOnly) {
            // if this is not the first pass (most local scope)
            // nor the last pass (global scope with function names)
            if ((firstPass == false) && (currentTable->next != NULL)) {
                // skip the checking
                shouldCheck = false;
            }
            firstPass = false;
        }

        // the actual checking
        HTabPair *pair = htab_find(currentTable, key);
        if (shouldCheck && (pair != NULL)) {
            return pair;
        }
        // move onto the next frame
        currentTable = currentTable->next;
    } while (currentTable != NULL);
    return NULL;
}

// Add an item into current table
Status st_add(SymTab *st, Token token, HTabPair **pair) {
    if (st_lookup(st, token.str, true) != NULL) {
        // If an item with the same key already exists as a function or a variable in the most local table, it's an error
        return ERR_SEMANTIC_DEF;
    } else {
        // If not found, add a new item and fill the information in
        *pair = htab_insert(st->top, token.str);
        (*pair)->value.varType = token_literal_to_type(token.type);
        (*pair)->value.ID = st->idCounter++;
        return SUCCESS;
    }
}

// Used when moving out of function / if / while
void st_pop_frame(SymTab *st) {
    HTab *deleted = st->top;
    st->top = st->top->next;
    htab_destroy(deleted);
}

// Symbol table destructor
void st_destroy(SymTab *st) {
    while (st->top) {
        HTab *deleted = st->top;
        st->top = st->top->next;
        htab_destroy(deleted);
    }
}

// Used to convert token type into type
Type st_token_to_type(SymTab *st, Token token) {
    // Check if token is type literal
    Type type = token_literal_to_type(token.type);
    if (type != TYPE_NONE) {
        return type;
    }

    // Check if token is a fn / var in symbol table
    HTabPair *pair = st_lookup(st, token.str, false);
    if (pair != NULL) {
        return pair->value.varType;
    }
    return TYPE_NONE;
}

// Checks if all declared functions are defined
Status st_check_fn_defined(SymTab *st) {
    if (htab_check_fn_define(st->top)) {
        return SUCCESS;
    }
    return ERR_SEMANTIC_DEF;
}
