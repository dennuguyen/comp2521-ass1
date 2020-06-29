// COMP2521 20T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Dict.h"
#include "stemmer.h"

#define STOPWORDS "stopwords"
#define MAXLINE 1000
#define MAXWORD 100

#define STARTING "*** START OF"
#define ENDING "*** END OF"

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')

extern Dict insert(Dict d, char *w);
extern int find(Dict d, char *w);
extern void getTopN(Dict d, Dict topN[], const int N);
extern void showTopN(Dict topN[], int N);
static Dict build_stopwords(void);
static Dict scan_file(char *fileName, Dict stopwords);
static char *normalise(char *word);

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
   Dict topN[nWords];
   for (int i = 0; i < nWords; i++)
      topN[i] = malloc(sizeof(Dict));

   getTopN(wfreqs, topN, nWords);
   showTopN(topN, nWords);

   return EXIT_SUCCESS;
}

static Dict build_stopwords(void)
{
   FILE *in = fopen(STOPWORDS, "r"); // open STOPWORDS file
   Dict stopwords = newDict();       // dictionary of stopwords

   if (!in)
   {
      fprintf(stderr, "Can't open stopwords\n");
      exit(EXIT_FAILURE);
   }

   // assume each word has its own line in stopFile
   char line[MAXLINE];
   while (fgets(line, sizeof(line), in))
      stopwords = insert(stopwords, normalise(line));

   fclose(in);

   return stopwords;
}

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
      if (strstr(line, STARTING) != NULL) // strstr returns pointer
         break;

   // could not find STARTING position
   if (strstr(line, STARTING) == NULL)
   {
      fprintf(stderr, "Not a Project Gutenberg book\n");
      exit(EXIT_FAILURE);
   }

   Dict wfreqs = newDict(); // dictionary of words from book

   // scan File reading words and accumulating counts
   char *delimiter = " ,.?!()[]\\/<>{}[]|=@#$%%^&*:;\"";
   while (fgets(line, sizeof(line), in) && strstr(line, ENDING) == NULL)
      for (char *word = strtok(line, delimiter); word != NULL; word = strtok(NULL, delimiter))
      {
         word = normalise(word);

         // if word is not a stopword and is not an empty string
         if (!find(stopwords, word) && strcmp(word, "") != 0 && strcmp(word, "-") != 0 && strcmp(word, "'") != 0)
         {
            int end = stem(word, 0, strlen(word) - 1);
            word[end + 1] = '\0';
            wfreqs = insert(wfreqs, word);
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