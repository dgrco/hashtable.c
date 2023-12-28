#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
  char *key;
  void *value;
} Entry;

typedef struct Node {
  Entry *entry;
  struct Node *next;
} Node;

typedef struct Hashtable {
  Node **buckets;
  size_t size;
  size_t capacity;
} Hashtable;

Hashtable *ht_create() {
  Hashtable *ht = (Hashtable *)calloc(1, sizeof(Hashtable));
  ht->size = 0;
  ht->capacity = 16;
  ht->buckets = (Node **)calloc(ht->capacity, sizeof(char *));
  return ht;
}

// Frees all contents of buckets, the buckets pointer itself, and the hashtable.
void ht_free(Hashtable *ht) {
  if (!ht)
    goto error;

  for (size_t i = 0; i < ht->capacity; i++) {
    if (ht->buckets[i]) {
      Node *curr = ht->buckets[i];
      while (curr) {
        Node *next = curr->next;
        if (curr->entry) {
          free(curr->entry);
        }
        free(curr);
        curr = next;
      }
    }
  }

  if (ht->buckets) {
    free(ht->buckets);
  }

  free(ht);
  return;

error:
  fprintf(stderr, "Hashtable does not exist.\n");
  exit(1);
}

size_t hash(const char *key) {
  unsigned int hash = 2166136261; // FNV-1a 32-bit offset basis
  for (const char *p = key; *p != '\0'; p++) {
    hash = (hash ^ (size_t)*p) * 16777619;
  }
  return hash;
}

void insert(Hashtable *ht, Node **buckets, size_t index, Node *node) {
  if (!buckets[index]) {
    buckets[index] = node;
    ht->size++;
    return;
  }

  Node *curr = buckets[index];
  Node *prev = NULL;
  while (curr) {
    if (strcmp(curr->entry->key, node->entry->key) == 0) {
      free(node->entry);
      free(node);
      return;
    }
    prev = curr;
    curr = curr->next;
  }
  prev->next = node;
}

void ht_insert(Hashtable *ht, char *key, void *value) {
  if (key[strlen(key) - 1] != '\0') {
    strncat(key, "\0", 1);
  }
  size_t hashed_key = hash(key);
  if ((double)ht->size / (double)ht->capacity >= 0.75) {
    // grow the table
    ht->capacity *= 2;
    // copy contents into new bucket list, update key hashes and indices
    Node **buckets = (Node **)calloc(ht->capacity, sizeof(Node *));
    for (size_t i = 0; i < ht->size; i++) {
      Node *curr = ht->buckets[i];
      while (curr != NULL) {
        Node *next = curr->next;
        size_t tmp_hashed_key = hash(curr->entry->key);
        insert(ht, buckets, tmp_hashed_key % ht->capacity, curr);
        curr = next;
      }
    }
    // free old buckets and reassign
    free(ht->buckets);
    ht->buckets = buckets;
  }
  // insert new element
  Node *new = (Node *)calloc(1, sizeof(Node));
  new->next = NULL;
  new->entry = (Entry *)calloc(1, sizeof(Entry));
  new->entry->key = key;
  new->entry->value = value;
  insert(ht, ht->buckets, hashed_key % ht->capacity, new);
}

void ht_delete(Hashtable *ht, char *key) {
  size_t hashed_key = hash(key);
  size_t index = hashed_key % ht->capacity;
  Node *curr = ht->buckets[index];
  while (curr) {
    if (curr->next && strcmp(curr->next->entry->key, key) == 0) {
      Node *next = curr->next->next;
      free(curr->next->entry);
      free(curr->next);
      curr->next = next;
      return;
    }
    curr = curr->next;
  }
}

void *ht_get(Hashtable *ht, char *key) {
  size_t hashed_key = hash(key);
  size_t index = hashed_key % ht->capacity;
  Node *curr = ht->buckets[index];
  while (curr) {
    if (strcmp(curr->entry->key, key) == 0) {
      return curr->entry->value;
    }
    curr = curr->next;
  }
  return NULL;
}

// Experimental, works only for <string, long> pairs
void ht_print(Hashtable *ht) {
  printf("{\n");
  for (size_t i = 0; i < ht->capacity; i++) {
    if (!ht->buckets[i]) {
      printf("\t{},\n");
      continue;
    }
    printf("\t{%s, %lu}", ht->buckets[i]->entry->key,
           *(long *)ht->buckets[i]->entry->value);
    Node *curr = ht->buckets[i]->next;
    while (curr) {
      printf("->{%s, %lu}", curr->entry->key, *(long *)curr->entry->value);
      curr = curr->next;
    }
    printf(",\n");
  }
  printf("}\n");
}
