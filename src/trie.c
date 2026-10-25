
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../headers/trie.h"

// Create a new trie node
TrieNode* createTrieNode(void)
{
    TrieNode* node = malloc(sizeof(TrieNode));

    if (node == NULL)
        return NULL;

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        node->children[i] = NULL;
    }

    node->isEnd = 0;

    return node;
}

// Insert a word into the trie
void trieInsert(TrieNode* root, const char* word)
{
    if (root == NULL || word == NULL)
        return;

    TrieNode* current = root;

    for (int i = 0; word[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)word[i];

        // Skip characters that are not letters
        if (!isalpha(c))
            continue;

        c = (unsigned char)tolower(c);

        // This trie supports English letters a-z only
        if (c < 'a' || c > 'z')
            continue;

        int index = c - 'a';

        if (current->children[index] == NULL)
        {
            current->children[index] = createTrieNode();

            if (current->children[index] == NULL)
                return;
        }

        current = current->children[index];
    }

    current->isEnd = 1;
}

// Search for a complete word
int trieSearch(TrieNode* root, const char* word)
{
    if (root == NULL || word == NULL)
        return 0;

    TrieNode* current = root;

    for (int i = 0; word[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)word[i];

        if (!isalpha(c))
            continue;

        c = (unsigned char)tolower(c);

        if (c < 'a' || c > 'z')
            continue;

        int index = c - 'a';

        if (current->children[index] == NULL)
            return 0;

        current = current->children[index];
    }

    return current->isEnd;
}

// Recursively collect words below a prefix
static void collect(
    TrieNode* node,
    char buffer[],
    int depth,
    char results[][MAX_WORD_LENGTH],
    int* count,
    int maxResults)
{
    if (node == NULL || *count >= maxResults)
        return;

    // If this node ends a word, save it
    if (node->isEnd)
    {
        buffer[depth] = '\0';

        strcpy(results[*count], buffer);
        (*count)++;
    }

    // Visit children alphabetically
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (*count >= maxResults)
            return;

        if (node->children[i] != NULL)
        {
            if (depth >= MAX_WORD_LENGTH - 1)
                continue;

            buffer[depth] = (char)('a' + i);

            collect(
                node->children[i],
                buffer,
                depth + 1,
                results,
                count,
                maxResults
            );
        }
    }
}

// Find all words starting with a prefix
int trieAutocomplete(
    TrieNode* root,
    const char* prefix,
    char results[][MAX_WORD_LENGTH],
    int maxResults)
{
    if (root == NULL || prefix == NULL ||
        results == NULL || maxResults <= 0)
    {
        return 0;
    }

    TrieNode* current = root;
    char cleanPrefix[MAX_WORD_LENGTH];
    int length = 0;

    // Normalize the prefix
    for (int i = 0; prefix[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)prefix[i];

        if (!isalpha(c))
            continue;

        c = (unsigned char)tolower(c);

        if (c < 'a' || c > 'z')
            continue;

        if (length >= MAX_WORD_LENGTH - 1)
            return 0;

        cleanPrefix[length++] = (char)c;
    }

    cleanPrefix[length] = '\0';

    // Follow the prefix through the trie
    for (int i = 0; i < length; i++)
    {
        int index = cleanPrefix[i] - 'a';

        if (current->children[index] == NULL)
            return 0;

        current = current->children[index];
    }

    // Start collecting words from the prefix node
    char buffer[MAX_WORD_LENGTH];

    strcpy(buffer, cleanPrefix);

    int count = 0;

    collect(
        current,
        buffer,
        length,
        results,
        &count,
        maxResults
    );

    return count;
}

// Free the trie recursively
void freeTrie(TrieNode* root)
{
    if (root == NULL)
        return;

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        freeTrie(root->children[i]);
    }

    free(root);
}
