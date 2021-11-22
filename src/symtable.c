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
        // If allocation fails
        return NULL;
    }

    else {
        table->arr_size = HTAB_SIZE;
        table->htab_items = malloc(sizeof(HTabItem *) * HTAB_SIZE);

        // Set the whole array to null
        for(size_t i = 0; i < HTAB_SIZE; i++) {
            table->htab_items[i] = NULL;
        }
    }
    table->next = NULL;
    return table;
}

// Hash function of a hash table
size_t htab_hash_function(HTabKey str) {
    unsigned h = 0; // 32 bits
    const unsigned char *p;

    for(p = (const unsigned char*)str; *p != '\0'; p++)
    {
        h = 65599*h + *p;
    }

    return h;
}

// Finds an element in a hash table, returns pointer to its pair
HTabPair *htab_find(HTab *table, HTabKey key) {
    unsigned key_hash = htab_hash_function(key) % table->arr_size;
    HTabItem *temp = table->htab_items[key_hash];

    while (temp)
    {
        if ((strcmp(temp->htab_pair.key, key)) == 0)
        {
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
    while (*element)
    {
        if (strcmp((*element)->htab_pair.key, key) == 0)
        {
            break;
        }

        // Move to next item of the list
        element = &(*element)->next;
    }

    *element = malloc(sizeof(HTabItem));
    if (*element == NULL)
    {
        return NULL;
    }

    (*element)->htab_pair.key = malloc(strlen(key) + 1);
    if ((*element)->htab_pair.key == NULL)
    {
        //allocation of item failed
        free(*element);
        return NULL;
    }

    // Initialize new item
    strcpy((char *)(*element)->htab_pair.key , key);
    TypeList paramList = list_init();
    TypeList returnList = list_init();

    (*element)->htab_pair.value.defined = false;
    (*element)->htab_pair.value.paramList = paramList;
    (*element)->htab_pair.value.returnList = returnList;
    (*element)->htab_pair.value.varType = TYPE_NONE;
    (*element)->next = NULL;

    //returning new item pair
    return (&(*element)->htab_pair);
}

// Deletes all elements of hash table, an empty allocated table is left behind
void htab_clear(HTab *table) {
    HTabItem *temp_element;

    // Delete items for every item in array
    for (unsigned i = 0; i < table->arr_size; i++)
    {
        // Delete the first item of a list
        temp_element = table->htab_items[i];
        table->htab_items[i]= NULL;

        // Free every item of a following list, its key
        while (temp_element != NULL)
        {
            HTabItem *current_element = temp_element->next;
            free((char *)temp_element->htab_pair.key);
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

Status st_init(SymTab *st) {
    st->top = htab_init();
    if (st->top == NULL) {
        return ERR_INTERNAL;
    }
    return SUCCESS;
}

Status st_push_frame(SymTab *st) {
    HTab *next = st->top;
    st->top= htab_init();
    if (st->top == NULL) {
        return ERR_INTERNAL;
    }

    st->top->next = next;
    return SUCCESS;
}

HTabPair *st_lookup(SymTab *st, const char *key) {
    HTab *currentTable = st->top;
    do {
        HTabPair *pair = htab_find(currentTable, key);
        if (pair) {
            return pair;
        }
        currentTable = currentTable->next;
    } while (currentTable);
    return NULL;
}

Status st_add(SymTab *st, Token token, HTabPair **pair) {
    if (st_lookup(st, token.str) != NULL) {
        return ERR_SEMANTIC_DEF;
    }
    else {
        *pair = htab_insert(st->top, token.str);
        (*pair)->value.varType = token_type_to_type(token.type);
        return SUCCESS;
    }
}

void st_pop_frame(SymTab *st) {
    HTab *deleted = st->top;
    st->top = st->top->next;
    htab_destroy(deleted);
}

void st_destroy(SymTab *st) {
    while (st->top) {
        HTab *deleted = st->top;
        st->top = st->top->next;
        htab_destroy(deleted);
    }
}

Type st_token_to_type(SymTab *st, Token token) {
    Type type = token_type_to_type(token.type);
    if (type != TYPE_NONE) {
        return type;
    }

    HTabPair *pair = st_lookup(st, token.str);
    if (pair != NULL) {
        return pair->value.varType;
    }

    return TYPE_NONE;
}
