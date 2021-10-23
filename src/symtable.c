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

 #include "symtable_private.h"

 // Constructor, creates an empty table, numb is size of the table
 htab_t *htab_init(size_t numb)
 {
     htab_t *h_table = malloc(sizeof(htab_t));

     if (h_table == NULL)
     {
         // If allocation fails
         return NULL;
     }
     else
     {
         h_table->arr_size = numb;
         h_table->size = 0;
         h_table->htab_items = malloc(sizeof(htab_item_t *) * numb);

         // Set all the fields of an array to null
         for(size_t i = 0; i < numb; i++)
         {
             h_table->htab_items[i] = NULL;
         }
     }

     return h_table;
 }

 // Returns size of hash table array
 size_t htab_bucket_count(const htab_t *t)
 {
     return t->arr_size;
 }

 // Deletes all elements of hash table, an empty allocated table is left behind
 void htab_clear(htab_t *t)
 {
     htab_item_t *temp_element;
     htab_item_t *current_element;
     t->size = 0;

     // Delete items for every item in array
     for (index_t i = 0; i < t->arr_size; i++)
     {
         // Delete the first item of a list
         temp_element = t->htab_items[i];
         t->htab_items[i]= NULL;

         // Free every item of a following list, its key
         while (temp_element != NULL)
         {
             current_element = temp_element->next;
             free((char *)temp_element->htab_pair.key);
             free(temp_element);
             temp_element = current_element;
         }
     }
     return;
 }

 // Finds an element of hash table, if found deletes it, returns true
 bool htab_erase(htab_t *t, htab_key_t key)
 {
     index_t element_index = htab_hash_function(key) % t->arr_size;
     htab_item_t **current_element = &t->htab_items[element_index];
     htab_item_t *deleted_element;

     while (*current_element != NULL)

     {
         if ((strcmp((*current_element)->htab_pair.key, key)) == 0)
         {
             deleted_element = *current_element;
             (*current_element) = (*current_element)->next;
             free((char *)deleted_element->htab_pair.key);
             free(deleted_element);
             t->size--;
             return true;
         }

         // Go to the next item of a list
         current_element = &(*current_element)->next;
     }

     // If not found return false
     return false;
 }

 // Finds an element in a hash table, returns pointer to its pair
 htab_pair_t *htab_find(htab_t *t, htab_key_t key)
 {
     index_t element_index = htab_hash_function(key) % t->arr_size;
     htab_item_t *temp = t->htab_items[element_index];
     htab_pair_t *ret;

     while (temp)
     {
         if (!(strcmp(temp->htab_pair.key, key)))
         {
             ret = &(temp->htab_pair);
             return ret;
         }
         temp = temp->next;
     }

     // If not found return null
     return NULL;
 }

 // Cycles through all the elements of a hash table and calls a function
 void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data))
 {
     htab_item_t *temp;
     for (index_t i = 0; i < t->arr_size; i++)
     {
         temp = t->htab_items[i];

         while (temp != NULL)
         {
             f(&temp->htab_pair);
             temp = temp->next;
         }
         free(temp);
     }

     return;
 }

 // Hash function of a hash table
 size_t htab_hash_function(htab_key_t str)
 {
     index_t h = 0; // 32 bits
     const unsigned char *p;

     for(p = (const unsigned char*)str; *p != '\0'; p++)
     {
         h = 65599*h + *p;
     }

     return h;
 }

 // Searches for an item, creates if not found
 htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key)
 {
     index_t hash_key = htab_hash_function(key) % t->arr_size;
     htab_item_t **element = &(t->htab_items[hash_key]);

     // Find last item of the list
     while (*element != NULL)
     {
         if (strcmp((*element)->htab_pair.key, key) == 0)
         {
             return (&(*element)->htab_pair);
         }

         // Move to next item of the list
         element = &(*element)->next;
     }

     if ((*element = malloc(sizeof(htab_item_t))) == NULL)
     {
         return NULL;
     }

     if (((*element)->htab_pair.key = malloc(strlen(key) + 1))  == NULL)
     {
         //allocation of item failed
         free(*element);
         return NULL;
     }

     // Initialize new item
     strcpy((char *)(*element)->htab_pair.key , key);
     (*element)->htab_pair.value = 0;
     (*element)->next = NULL;

     t->size++;

     //returning new item pair
     return (&(*element)->htab_pair);
 }

 // Constructor, creates and fills table with data from an existing table
 htab_t *htab_move(size_t newnumb, htab_t *t2)
 {
     htab_t *new_table;

     if ((new_table = htab_init(newnumb)) == NULL)
     {
         return NULL;
     }

     for (size_t i = 0; i < t2->arr_size; i++)
     {
         htab_item_t *temp_item = t2->htab_items[i];
         htab_pair_t *temp_pair;

         while (temp_item)
         {
             temp_pair = htab_lookup_add(new_table, temp_item->htab_pair.key);

             if (temp_pair == NULL)
             {
                 return NULL;
             }

             temp_pair->value = temp_item->htab_pair.value;
             temp_item = temp_item->next;
         }
     }

     // Delete content of the old table
     htab_clear(t2);

     return new_table;
 }

 // Returns number of elements of hash table
 size_t htab_size(const htab_t *t)
 {
     return t->size;
 }

 // Destructor of a hash table
 void htab_free(htab_t *t)
 {
     htab_clear(t);
     free(t->htab_items);
     t->arr_size = 0;
     t->size = 0;
     free(t);
     return;
 }
