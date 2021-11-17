/**
 * @file symtable.c
 *
 * @author Hnatovskyj Vítek xhnato00
 * @author Lán Rostislav xlanro00
 * @author Mach Ondřej xmacho12
 * @author Slivka Matej xslivk03
 *
 * IFJ projekt 2021
 */

// The source of  this file is IJC project 2 2020/2021 (xlanro00)

 #include "symtable_private.h"

 // Constructor, creates an empty table
 SymTab *symtab_init() {
     SymTab *table = malloc(sizeof(SymTab));

     if (table == NULL)
     {
         // If allocation fails
         return NULL;
     }

     else
     {
         table->arr_size = SYMTAB_SIZE;
         table->symtab_items = malloc(sizeof(SymTabItem *) * SYMTAB_SIZE);

         // Set the whole array to null
         for(size_t i = 0; i < SYMTAB_SIZE; i++)
         {
             table->symtab_items[i] = NULL;
         }
     }

     return table;
 }

 // Finds an element in a hash table, returns pointer to its pair
 SymTabPair *symtab_find(SymTab *table, SymTabKey key) {
     unsigned key_hash = symtab_hash_function(key) % table->arr_size;
     SymTabItem *temp = table->symtab_items[key_hash];

     while (temp)
     {
         if ((strcmp(temp->symtab_pair.key, key)) == 0)
         {
             return &(temp->symtab_pair);
         }
         temp = temp->next;
     }

     return NULL;
 }

 // Hash function of a hash table
 size_t symtab_hash_function(SymTabKey str) {
     unsigned h = 0; // 32 bits
     const unsigned char *p;

     for(p = (const unsigned char*)str; *p != '\0'; p++)
     {
         h = 65599*h + *p;
     }

     return h;
 }

 // Searches for an item, creates if not found
 SymTabPair *symtab_insert(SymTab *table, SymTabKey key, SymTabValue value) {
     unsigned key_hash = symtab_hash_function(key) % table->arr_size;
     SymTabItem **element = &(table->symtab_items[key_hash]);

     // Find last item of the list
     while (*element)
     {
         if (strcmp((*element)->symtab_pair.key, key) == 0)
         {
             break;
         }

         // Move to next item of the list
         element = &(*element)->next;
     }

     *element = malloc(sizeof(SymTabItem));
     if (*element == NULL)
     {
         return NULL;
     }

     (*element)->symtab_pair.key = malloc(strlen(key) + 1);
     if ((*element)->symtab_pair.key == NULL)
     {
         //allocation of item failed
         free(*element);
         return NULL;
     }

     // Initialize new item
     strcpy((char *)(*element)->symtab_pair.key , key);
     (*element)->symtab_pair.value = value;
     (*element)->next = NULL;

     //returning new item pair
     return (&(*element)->symtab_pair);
 }

// Deletes all elements of hash table, an empty allocated table is left behind
void symtab_clear(SymTab *table) {
    SymTabItem *temp_element;


    // Delete items for every item in array
    for (unsigned i = 0; i < table->arr_size; i++)
    {
        // Delete the first item of a list
        temp_element = table->symtab_items[i];
        table->symtab_items[i]= NULL;

        // Free every item of a following list, its key
        while (temp_element != NULL)
        {
            SymTabItem *current_element = temp_element->next;
            free((char *)temp_element->symtab_pair.key);
            free(temp_element);
            temp_element = current_element;
        }
    }
}

 // Destructor of a hash table
 void symtab_destroy(SymTab *table) {
     symtab_clear(table);
     free(table->symtab_items);
     table->arr_size = 0;
     free(table);
 }
