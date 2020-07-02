// COMP2521 20T2 Assignment 1
// Dict.c ... implementsation of Dictionary ADT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"

#define FALSE 0
#define TRUE !FALSE

typedef struct _DictRep
{
    char *word;
    int freq;
    struct _DictRep *left;
    struct _DictRep *right;
} Word;

/**
 * Converts a char* w into a hashed unsigned long using djb2 algorithm.
 * 
 * Significantly faster than strcmp for string comparison:
 *      data/2701.txt
 *      strcmp - 1.200s
 *      hashing - 0.380s
 */
static unsigned long djb2(char *w)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *w++))
        hash = (hash << 5) + hash + c;

    return hash;
}

/**
 * Create a new word node to store char *word, int frequency, Word *left and
 * Word *right.
 */
static Word *newWord(char *w)
{
    Word *word = malloc(sizeof(Word));
    if (word == NULL)
    {
        fprintf(stderr, "Could not allocate memory for word struct\n");
        exit(EXIT_FAILURE);
    }

    word->word = strdup(w); // strdup has hidden malloc
    if (word->word == NULL)
    {
        fprintf(stderr, "Could not allocate memory for word->word\n");
        exit(EXIT_FAILURE);
    }

    word->freq = 1;
    word->left = word->right = NULL;

    return word;
}

/**
 * Returns NULL.
 */
Dict newDict()
{
    return NULL;
}

/**
 * Insert a word into the dictionary given by char* w. Creates a new Word node
 * if it does not exist in the dictionary.
 * 
 * Returns a Word*.
 */
Word *insert(Word *d, char *w)
{
    if (d == NULL)
        return newWord(w);

    unsigned long w_hash = djb2(w);
    unsigned long d_hash = djb2(d->word);

    if (w_hash < d_hash)
        d->left = insert(d->left, w);
    else if (w_hash > d_hash)
        d->right = insert(d->right, w);
    else // (w_hash == d_hash)
        d->freq++;

    return d;
}

/**
 * Honouring Dict.h
 * 
 * This function cannot be honoured as Dict d must be passed by pointer to
 * update the dictionary that was declared in tw.c, i.e.:
 * 
 *      WFreq *DictInsert(Dict *d, char *w)
 *      WFreq *DictInsert(struct _DictRep &d, char *w)
 * 
 * or the function will return a Dict where an assignment can occur in tw.c,
 * i.e.:
 * 
 *      Dict DictInsert(Dict d, char *w)
 * 
 * or use a second struct to hold a pointer to the tree.
 */
WFreq *DictInsert(Dict d, char *w)
{
    d = insert(d, w); // this will not update d declared in tw.c as it is local
    return NULL;
}

/**
 * Finds the specified word in the dictionary given by char* w.
 * 
 * Returns FALSE if word is not found.
 */
static int find(Word *d, char *w)
{
    if (d == NULL)
        return FALSE;

    unsigned long w_hash = djb2(w);
    unsigned long d_hash = djb2(d->word);

    if (w_hash < d_hash)
        return find(d->left, w);
    else if (w_hash > d_hash)
        return find(d->right, w);
    else // (w_hash == d_hash)
        return TRUE;
}

/**
 * Honouring Dict.h
 */
WFreq *DictFind(Dict d, char *w)
{
    WFreq *placeholder = &(WFreq){.freq = 0, .word = ""};
    return find(d, w) ? placeholder : NULL;
}

/**
 * Find top N frequently occurring words in a Dict and stores them in
 * alphabetical order.
 */
static void getTopN(Word *d, Word *topN[], int N)
{
    if (d == NULL || topN == NULL || N == 0)
        return;

    // iterate through topN comparing current word and top N words
    for (int i = 0; i < N; i++)
    {
        // empty element
        if (topN[i] == NULL)
        {
            topN[i] = newWord(d->word);
            topN[i]->freq = d->freq;
            break;
        }

        // insert Dict into topN
        if ((topN[i]->freq == d->freq && strcmp(d->word, topN[i]->word) < 0) ||
            topN[i]->freq < d->freq)
        {
            // shift topN array by 1 from i
            memmove(&topN[i + 1], &topN[i], (N - i - 1) * sizeof(topN[0]));

            // insert into topN at i
            topN[i] = d;
            break;
        }
    }

    getTopN(d->left, topN, N);
    getTopN(d->right, topN, N);
}

/**
 * Print index 0 to N-1 of topN array.
 */
static void showTopN(Word *topN[], int N)
{
    if (topN == NULL)
        return;

    for (int i = 0; i < N; i++)
        printf("%*d %s\n", 7, topN[i]->freq, topN[i]->word);
}

/**
 * Honouring Dict.h
 */
int findTopN(Dict d, WFreq *wfs, int n)
{
    Dict topN[n];
    for (int i = 0; i < n; i++)
        topN[i] = newDict();
    getTopN(d, topN, n);
    showTopN(topN, n);
    return EXIT_SUCCESS;
}

/**
 * Prints the dictionary.
 */
void showDict(Dict d)
{
    if (d == NULL)
        return;

    showDict(d->left);
    printf("%s ", d->word);
    showDict(d->right);
}
