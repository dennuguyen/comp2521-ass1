// COMP2521 20T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File

/**
 * PLAGIARISM NOTICE
 * 
 * The implementation of the following function:
 * 
 * line 235:    static char *strtok_t(char *str, int (*test)(int ch));
 * 
 * has been used from the following external intellectual source:
 * 
 * AUTHOR: BLUEPIXY
 * LICENSE: CC BY-SA 3.0
 * URL: https://stackoverflow.com/a/26243667/13133452
 * COMMENTS: Brace styling and variable names changed from original code.
 * 
 * I acknowledge the implementation of this function is NOT an adequate
 * solution of the assignment and is compliant with CSE's plagiariasm policy.
 * 
 * "it is important to understand that although appropriate attribution
 * implies that derived work does not constitute plagiarism, it generally also
 * implies that the work is not an adequate solution of the assignment."
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "Dict.h"
#include "stemmer.h"

#define STOPWORDS "stopwords"
#define RANDSTOPWORDS "stopwords.random"
#define MAXLINE 1000
#define MAXWORD 100

#define STARTING "*** START OF"
#define ENDING "*** END OF"

// change from #define to function to be able to use as function pointer
static int isWordChar(int c) { return isalnum(c) || c == '\'' || c == '-'; }
static Dict build_stopwords(void);
static Dict scan_file(char *fileName, Dict stopwords);
static char *normalise(char *word);
static char *strtok_t(char *str, int (*func)(int ch));

int main(int argc, char *argv[])
{
    // get fileName and nWords
    char *fileName;
    int nWords; // number of top frequency words to show
    switch (argc)
    {
    case 2:
        nWords = 10;
        fileName = argv[1];
        break;
    case 3:
        nWords = atoi(argv[1]);
        if (nWords < 10)
            nWords = 10;
        fileName = argv[2];
        break;
    default:
        fprintf(stderr, "Usage: %s [Nwords] File\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // build stopword dictionary
    Dict stopwords = build_stopwords();

    // get word frequencies
    Dict wfreqs = scan_file(fileName, stopwords);

    // check if wfreqs is empty
    if (wfreqs == NULL)
        return EXIT_SUCCESS;

    // debugging
    // showDict(wfreqs);

    // compute and display the top N words
    findTopN(wfreqs, NULL, nWords);

    return EXIT_SUCCESS;
}

/**
 * Create a randomised stopwords file from the stopwords file and store in
 * Dict stopwords.
 * 
 * Assume each word in stopwords is on its own line.
 */
static Dict build_stopwords(void)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "sort -R %s > %s", STOPWORDS, RANDSTOPWORDS);
    system(buffer);                       // create randomised stopwords
    FILE *in = fopen(RANDSTOPWORDS, "r"); // open STOPWORDS file
    Dict stopwords = newDict();           // dictionary of stopwords

    if (!in)
    {
        fprintf(stderr, "Can't open stopwords\n");
        exit(EXIT_FAILURE);
    }

    // get the stop words and insert it into the dictionary
    char line[MAXLINE];
    while (fgets(line, sizeof(line), in))
        DictInsert(stopwords, normalise(line));

    fclose(in);
    snprintf(buffer, sizeof(buffer), "rm %s", RANDSTOPWORDS);
    system(buffer); // destroy randomised stopwords

    return stopwords;
}

/**
 * Scan the book and insert words if it is not a stopword. Recurrent words
 * are incremened by freq instead of insertion.
 */
static Dict scan_file(char *fileName, Dict stopwords)
{
    FILE *in = fopen(fileName, "r"); // open book

    if (!in)
    {
        fprintf(stderr, "Can't open %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    char line[MAXLINE]; // current input line

    // scan File, up to start of text
    while (fgets(line, sizeof(line), in))
        if (strstr(line, STARTING) != NULL) // strstr !NULL if found
            break;

    // could not find STARTING position
    if (strstr(line, STARTING) == NULL)
    {
        fprintf(stderr, "Not a Project Gutenberg book\n");
        exit(EXIT_FAILURE);
    }

    Dict wfreqs = newDict(); // dictionary of words from book

    // scan the file
    while (fgets(line, sizeof(line), in) && strstr(line, ENDING) == NULL)
        for (char *word = strtok_t(line, isWordChar); word != NULL;
             word = strtok_t(NULL, isWordChar))
        {
            word = normalise(word);

            if (strlen(word) <= 1) // skip words of length 1 or 0
                continue;
            else if (!DictFind(stopwords, word))
            {
                int end = stem(word, 0, strlen(word) - 1);
                word[end + 1] = '\0';
                DictInsert(wfreqs, word);
            }
        }

    fclose(in);

    return wfreqs;
}

/**
 * Normalises the word by allowing valid chars and lower-casing the string.
 */
static char *normalise(char *word)
{
    int i = 0, j = 0;
    char c;
    while ((c = word[i++]) != '\0')
        if (isWordChar(c))
            word[j++] = tolower(c);
    word[j] = '\0';

    return word;
}

/**
 * Alternative strtok function to pass isWordChar function pointer.
 * 
 * AUTHOR: BLUEPIXY
 * LICENSE: CC BY-SA 3.0
 * URL: https://stackoverflow.com/a/26243667/13133452
 * COMMENTS: Brace styling and variable naming changed from original code.
 */
static char *strtok_t(char *str, int (*func)(int c))
{
    static char *store = NULL;
    char *word;

    if (str != NULL)
        store = str;

    if (store == NULL)
        return NULL;

    while (*store && !func(*store))
        ++store;

    if (*store == '\0')
        return NULL;

    word = store;
    while (*store && func(*store))
        ++store;

    if (*store == '\0')
        store = NULL;
    else
        *store++ = '\0';

    return word;
}
