#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "struct.h"

typedef struct PersonNode { // instead of writing struct personnode we can dorectly write personnode
    int personId;
    struct Person person;
    struct PersonNode *next;
} PersonNode;

// PersonNode will look like this
// ┌─────────────────────────┐
// │ personId = 101          │
// ├─────────────────────────┤
// │ Person                  │
// │  id = 101               │
// │  name = "Rahul"         │
// │  role = "suspect"       │
// │  age = 25               │
// │  contact = ...          │
// │  notes = ...            │
// └─────────────────────────┘

//hash table structure

typedef struct{
    PersonNode **table; // pointer to an array of PersonNode pointers
    int size; // size of the hash table
    int count; // number of elements in the hash table
} HashTable;

// PersonNode*  = pointer to one PersonNode
// PersonNode** = pointer to an array of PersonNode pointers

// fucntions we'll implement in hash_table.c
HashTable* createHashTable(int size); // will return address of newly cretaed hashtable
void insertPerson(HashTable *ht, struct Person person); // ht the hashtable where we want to insert the person, person is the person we want to insert 
struct Person* searchPerson(HashTable *ht, int personId); // will return address of the person if found, else NULL
void deletePerson(HashTable *ht, int personId); // will delete the person with the given ID from the hash table
void freeHashTable(HashTable *ht); // will free the memory allocated for the hash table
// it prevent memory leak
#endif

            //       Person
            //         │
            //         ▼
            //  insertPerson()
            //         │
            //         ▼
            //   Hash Function
            //         │
            //         ▼
            //  Find table index
            //         │
            //         ▼
            //  PersonNode stored
            //         │
            //         ▼
            //  ┌───────────────┐
            //  │ Hash Table    │
            //  ├───────────────┤
            //  │ 0 → NULL      │
            //  │ 1 → Person    │
            //  │ 2 → NULL      │
            //  │ 3 → Person    │
            //  │ 4 → NULL      │
            //  └───────────────┘