#include <stdio.h>
#include "headers/trie.h"

int main(void)
{
    TrieNode* root = createTrieNode();

    if (root == NULL)
    {
        printf("Trie creation failed!\n");
        return 1;
    }

    trieInsert(root, "rahul");
    trieInsert(root, "ravi");
    trieInsert(root, "raj");
    trieInsert(root, "amit");

    printf("Search rahul: %s\n",
           trieSearch(root, "rahul") ? "PASS" : "FAIL");

    printf("Search rah: %s\n",
           trieSearch(root, "rah") ? "PASS" : "FAIL");

    char results[10][MAX_WORD_LENGTH];

    int count = trieAutocomplete(
        root, "ra", results, 10
    );

    printf("\nAutocomplete 'ra':\n");

    for (int i = 0; i < count; i++)
    {
        printf("%s\n", results[i]);
    }

    printf("\nAutocomplete 'am':\n");

    count = trieAutocomplete(root, "am", results, 10);

    for (int i = 0; i < count; i++)
    {
        printf("%s\n", results[i]);
    }

    freeTrie(root);

    return 0;
}
