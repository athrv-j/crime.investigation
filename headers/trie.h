#ifndef TRIE_H
#define TRIE_H

#define ALPHABET_SIZE 26
#define MAX_WORD_LENGTH 100

typedef struct TrieNode{
    struct TrieNode* children[ALPHABET_SIZE];
    int isEnd;
} TrieNode;

// create new trie node
TrieNode* createTrieNode(void);

// insert a word
void trieInsert(TrieNode* root, const char* word);

// search 
int trieSearch(TrieNode* root, const char* word);

// Find words matching a prefix
int trieAutocomplete(
    TrieNode* root,
    const char* prefix,
    char results[][MAX_WORD_LENGTH],
    int maxResults
);

// Free the trie
void freeTrie(TrieNode* root);

#endif