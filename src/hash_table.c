
#include <stdio.h>
#include <stdlib.h>
#include "../headers/hash_table.h"

static int hash(int key, int size)
{  if (size <= 0)
        return 0;

    int index = key % size;

    if (index < 0)
        index += size;

    return index;
}

HashTable* createHashTable(int size)
{
    if (size <= 0)
        return NULL;

    HashTable* ht = malloc(sizeof(HashTable));

    if (ht == NULL)
        return NULL;

    ht->table = malloc(size * sizeof(PersonNode*));

    if (ht->table == NULL)
    {
        free(ht);
        return NULL;
    }

    ht->size = size;
    ht->count = 0;

    for (int i = 0; i < size; i++)
    {
        ht->table[i] = NULL;
    }

    return ht;
}

void insertPerson(HashTable* ht, struct Person person)
{
    if (ht == NULL || ht->size <= 0 || person.id < 0)
        return;

      /* Do not insert duplicate IDs */
    if (searchPerson(ht, person.id) != NULL)
        return;

    int index = hash(person.id, ht->size);

    PersonNode* newNode = malloc(sizeof(PersonNode));

    if (newNode == NULL)
        return;

    newNode->personId = person.id;
    newNode->person = person;

    // Insert at the beginning of the linked list
    newNode->next = ht->table[index];
    ht->table[index] = newNode;

    ht->count++;
}

struct Person* searchPerson(HashTable* ht, int personId)
{
    if (ht == NULL || ht->size <= 0)
        return NULL;

    int index = hash(personId, ht->size);
    PersonNode* current = ht->table[index];

    while (current != NULL)
    {
        if (current->personId == personId)
        {
            return &(current->person);
        }

        current = current->next;
    }

    return NULL;
}

void deletePerson(HashTable* ht, int personId)
{
    if (ht == NULL || ht->size <= 0)
        return;

    int index = hash(personId, ht->size);

    PersonNode* current = ht->table[index];
    PersonNode* previous = NULL;

    while (current != NULL)
    {
        if (current->personId == personId)
        {
            if (previous == NULL)
            {
                // Delete the first node
                ht->table[index] = current->next;
            }
            else
            {
                // Delete a middle or last node
                previous->next = current->next;
            }

            free(current);
            ht->count--;

            return;
        }

        previous = current;
        current = current->next;
    }
}

void freeHashTable(HashTable* ht)
{
    if (ht == NULL)
        return;

    for (int i = 0; i < ht->size; i++)
    {
        PersonNode* current = ht->table[i];

        while (current != NULL)
        {
            PersonNode* temp = current;
            current = current->next;

            free(temp);
        }
    }

    free(ht->table);
    free(ht);
}
