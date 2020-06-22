// COMP2521 20T2 Assignment 1
// Dict.c ... implementsation of Dictionary ADT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dict.h"

#include <assert.h>

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
 *      data/0011.txt
 *      strcmp - 0.094s
 *      hashing - 0.047s
 * 
 *      data/2701.txt
 *      strcmp - 0.745s
 *      hashing - 0.412s
 */
static unsigned long djb2(char *w)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *w++))
        hash = (hash << 5) + hash + c;

    return hash;
}

static void insertionSort(Dict topN[], int N)
{
    int i, element, j;
    for (i = 1; i < N; i++)
    {
        element = topN[i];
        j = i - 1;
        while (j >= 0 && topN[j] > element)
        {
            topN[j + 1] = topN[j];
            j = j - 1;
        }
        topN[j + 1] = element;
    }
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
        fprintf(stderr, "Could not allocate memory for new word\n");
        exit(EXIT_FAILURE);
    }

    word->word = strdup(w); // make copy as original char* will be destroyed
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
Dict insert(Dict d, char *w)
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
 * Finds the specified word in the dictionary given by char* w.
 * 
 * Returns NULL if word is not found.
 * 
 * Note: return destroys the stack therefore "bubbling" the answer through recursion.
 */
int find(Dict d, char *w)
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
 * Find top N frequently occurring words in a Dict.
 */
void getTopN(Dict d, Dict topN[], int N)
{
    if (d == NULL || topN == NULL || N == 0)
        return;

    // iterate through topN comparing current word and top N words
    for (int i = 0; i < N; i++)
    {
        // empty element
        if (topN[i] == NULL)
        {
            topN[i] = d;
            break;
        }

        if (topN[i]->freq < d->freq)
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
 * Print index 0 to N of topN array.
 * 
 * showTopN is defined in Dict.c due to C's inherent design to prevent access
 * to topN members in main.c.
 */
void showTopN(Dict topN[], int N)
{
    if (topN == NULL)
        return;

    // insertion sort is good for nearly sorted data
    insertionSort(topN, N);

    for (int i = 0; i < N; i++)
        printf("\t %d %s\n", topN[i]->freq, topN[i]->word);
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

Dict freeDict(Dict w)
{
    if (w == NULL)
        return NULL;

    else if (w->left == NULL && w->right == NULL)
    {
        free(w->word);
        free(w);
        return NULL;
    }
    else if (w->left == NULL && w->right != NULL)
    {
        Word *tmp = w->right;
        free(w->word);
        free(w);
        return tmp;
    }
    else if (w->left != NULL && w->right == NULL)
    {
        Word *tmp = w->left;
        free(w->word);
        free(w);
        return tmp;
    }
    else
    {
        Dict parent = w;
        Dict succ = w->right; // not null!
        while (succ->left != NULL)
        {
            parent = succ;
            succ = succ->left;
        }
        w->word = succ->word;
        free(succ);
        if (parent == w)
            parent->right = succ->right;
        else
            parent->left = succ->right;
        return w;
    }
}