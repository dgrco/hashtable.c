#ifndef HASHTABLE
#define HASHTABLE

#include <stdlib.h>

typedef struct Hashtable Hashtable;

Hashtable* ht_create();
void ht_free(Hashtable *ht);

void ht_insert(Hashtable *ht, char *key, void *value);
void ht_delete(Hashtable *ht, char *key);

// Returns the value of `key` if it exists, otherwise return `NULL`
void *ht_get(Hashtable *ht, char *key);

// Experimental, use only if pairs are of the form <string, long>
void ht_print(Hashtable *ht);

#endif
