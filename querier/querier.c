/* 
 * quierer.c - 'querier' module for use in the Tiny Search Engine (TSE) project
 *
 * William Dinauer, Dartmouth CS50 Winter 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "string.h"
#include "hashtable.h"
#include "set.h"
#include "counters.h"
#include "file.h"
#include "pagedir.h"
#include "word.h"
#include "index.h"

/* ***************** structs ********************** */
struct twoctrs {
    counters_t *ctrs1; 
    counters_t *ctrs2;
};

typedef struct pair {
    int docId;
    int count;
} pair_t;

/* ***************** functions ********************** */
bool validateParameters(int argc, char *argv[]);
bool querier(char *argv[]);
bool splitline(char *line, char *words[], int *numWords);
counters_t *searchMatches(hashtable_t *index, char *words[], int numWords);
void keycount(void *arg, int key, int count);
void printResults(counters_t *result, int keyCount, char *crawlDir);
void sortPairs(void *arg, int key, int count);
bool isand(char *word);
bool isor(char *word);
void andCounters(counters_t *counts, counters_t *andSequence);
void intersectCounts(void *arg, int key, int count);
void mergeCounters(counters_t *counts, counters_t *result);
void unionCounts(void *arg, int key, int count);
bool isZeros(pair_t *pr);
static void ctrs_delete(void *ctrs);

/* ***************** main ********************** */
int main(int argc, char *argv[])
{
    // validate parameters
    if (!validateParameters(argc, argv)) {
        return 1;
    }
    
    //run the querier
    if (!querier(argv)) {
        return 1;
    }

    return 0;
}

/* ***************** querier ********************** */
bool querier(char *argv[])
{
    char *line;
    hashtable_t *index = loadIndex(argv[2]);
    if (index == NULL) {
        fprintf(stderr, "unable to load index from the index file; out of memory\n");
        return false;
    }
    //TODO: REPLACE THIS W LECTURE CODE
    printf("Query? ");
    while((line = freadlinep(stdin)) != NULL) {
        int maxWords;
        int len = strlen(line);
        if (len % 2 == 0){
            maxWords = len/2;
        } else {
            maxWords = (len+1)/2;
        }
        if (maxWords > 0) {
            int numWords = 0;
            char *words[maxWords];
            if (splitline(line, words, &numWords) && numWords > 0) {
                printf("Query:");
                int i;
                for (i = 0; i < numWords; i++) {
                    printf(" %s", words[i]);
                }
                fputc('\n', stdout);

                counters_t *result = searchMatches(index, words, numWords);
                if (result != NULL) {
                    int keyCount = 0;
                    counters_iterate(result, &keyCount, keycount);
                    if (keyCount == 0) {
                        printf("No documents match.\n");
                    } else {
                        printf("Matches %d documents (ranked):\n", keyCount);
                        printResults(result, keyCount, argv[1]);
                    }
                    counters_delete(result);
                }
            }
        }
        // TODO: REPLACE WITH LECTURE CODE
        printf("Query? ");
        free(line);
        line = NULL;
    }
    fputc('\n', stdout);
    hashtable_delete(index, ctrs_delete);
    return true; 
}

/* ***************** printResults ********************** */
/* function to print the resulting counters with all 
 * (docId, score) pairs.
 *
 * Sorts the counters and prints (key, count) pairs in the form:
 * score: (count) doc (key): (url)
 * Where the url is the url for the webpage of the associated docId
 */
void printResults(counters_t *result, int keyCount, char *crawlDir)
{
    const pair_t zeros = {0, 0}; 
    pair_t pairs[keyCount];
    int i;
    for (i = 0; i < keyCount; i++) {
        pairs[i] = zeros;
    }

    counters_iterate(result, &pairs, sortPairs);

    for (i = 0; i < keyCount; i++) {
        pair_t *dirCt = &pairs[i];
        int count = dirCt->count;
        int key = dirCt->docId;

        webpage_t *wp = getNextWebpage(crawlDir, &key);
        char *url = webpage_getURL(wp);
        printf("score: %3d doc %4d: %s\n", count, key, url);
        webpage_delete(wp);
    }
}

/* ***************** sortPairs ********************** */
/* function to be passed to counters_iterate
 * to insert a given node of the counters into the
 * array of pairs, in sorted order.
 */
void sortPairs(void *arg, int key, int count)
{
    pair_t *pairs = arg;
    pair_t pr = {key, count};

    int i = 0;
    while (true) {
        pair_t *dirCt = &pairs[i];
        if (dirCt->count < count) {
            if (isZeros(dirCt)) {
                pairs[i] = pr;
                break;
            } else {
                int j = i + 1;
                pair_t *next = &pairs[j];
                while (!isZeros(next)) {
                    int nextCount = next->count;
                    int nextId = next->docId;
                    pairs[j] = *dirCt;
                    dirCt->count = nextCount;
                    dirCt->docId = nextId;
                    j++;
                    next = &pairs[j];
                }
                pairs[j] = *dirCt;
                pairs[i] = pr;
                break;
            }
        }
        i++;
    }
}

/* ***************** isZeros ********************** */
/* simple function to check if a pair is of the form {0, 0}
 *
 * Note: while we do not explicitly check that the docId/key
 * is 0, it is implied to be 0 if the count is 0 as any valid
 * key/docId will have a count > 0. pairs were initialized as {0, 0}.
 */
bool isZeros(pair_t *pr)
{
    if (pr->count == 0) {
        return true;
    }
    return false;
}

/* ***************** splitline ********************** */
/* function to split the query line into an array of words.
 * We return: 
 *      - false if the query has a non-alpha or non-space character.
 *      - true if the query can be split in a valid manner
 * Note: after a call to the function, words[] is updated to contain
 * the array of all words in a sequential manner, and numWords
 * is updated to the number of words in words[].
 */
bool splitline(char *line, char *words[], int *numWords)
{
    int begPos = 0;
    int endPos = 0;
    char *beg = line;

    NormalizeWord(line);

    while (true) {
        while (isspace(line[begPos])) {
            begPos++;
        }
        if (line[begPos] == '\0') {
            break;
        }
        beg = &line[begPos];
        endPos = begPos;
        while (isalpha(line[endPos])) {
            endPos++;
        }
        if (line[endPos] == '\0') {
            words[*numWords] = beg;
            (*numWords)++;
            break;
        }
        if (!isspace(line[endPos])) {
            fprintf(stderr, "bad character: '%c' in query\n", line[endPos]);
            return false;
        }
        line[endPos] = '\0';
        begPos = endPos + 1;
        words[*numWords] = beg;
        (*numWords)++;
    }
    return true;
}

/* ***************** searchMatches ********************** */
/* Searches the index for matches given the provided query.
 * Prints an error message if the query has invalid syntax.
 * Returns: 
 *      - the resulting counters module with all docId, count
 *      pairs, where each count is the associated score for the
 *      given webpage OR
 *      - NULL if the query has invalid syntax, or we run out of memory
 */
counters_t *searchMatches(hashtable_t *index, char *words[], int numWords)
{
    counters_t *result = counters_new();
    counters_t *andSequence = NULL;
    if (result == NULL) {
        return NULL;
    }

    bool wasConj = true;
    int i;
    for (i = 0; i < numWords; i++) {
        char *word = words[i];
        if (isand(word)) {
            if (wasConj || i == numWords - 1) {
                fprintf(stderr, "invalid query\n");
                counters_delete(result);
                counters_delete(andSequence);
                return NULL;
            } else {
                wasConj = true;
            }
        } else {
            if (isor(word)) {
                if (wasConj || i == numWords - 1 ) {
                    fprintf(stderr, "invalid query\n");
                    counters_delete(result);
                    counters_delete(andSequence);
                    return NULL;
                } else {
                    wasConj = true;
                    mergeCounters(andSequence, result);
                    counters_delete(andSequence);
                    andSequence = NULL;
                }
            } else {
                wasConj = false;
                counters_t *counts = hashtable_find(index, word);
                 if (andSequence == NULL) {
                    andSequence = counters_new();
                    if (andSequence == NULL) {  // out of memory
                        fprintf(stderr, "out of memory\n");
                        return NULL;
                    }
                    mergeCounters(counts, andSequence);
                 } else {
                    andCounters(andSequence, counts);
                 }
            }
        }
    }
    mergeCounters(andSequence, result);
    counters_delete(andSequence);
    return result;
}

/* ***************** isand ********************** */
/* simple function to check if a given word is "and"
 */
bool isand(char *word) 
{
    if (strcmp(word, "and") == 0) {
        return true;
    }
    return false;
}

/* ***************** isor ********************** */
/* simple function to check if a given word is "or"
 */
bool isor(char *word)
{
    if (strcmp(word, "or") == 0) {
        return true;
    }
    return false;
}

/* ***************** addCounters ********************** */
/* intersects two counters, with andSequence being
 * the resulting counters.
 * constructs the struct for use in counters_iterate
 * and calls counters_iterate with function intersectCounts
 */
void andCounters(counters_t *andSequence, counters_t *counts)
{
    struct twoctrs ctrs = {andSequence, counts};
    counters_iterate(andSequence, &ctrs, intersectCounts);
}

/* ***************** mergeCounters ********************** */
/* unions two counters, building into result.
 * calls counters_iterate with function unionCounts.
 */
void mergeCounters(counters_t *counts, counters_t *result)
{
    counters_iterate(counts, result, unionCounts);
}

/* ***************** intersectCounts ********************** */
/* intersects two counters, building up the andSequence
 */
void intersectCounts(void *arg, int key, int count)
{
    struct twoctrs *ctrs = arg;
    counters_t *andSequence = ctrs->ctrs1;
    counters_t *counts = ctrs->ctrs2;

    // compare the count in 'counts' (from the index) to the current count;
    // keep the minimum
    int potentialCount = counters_get(counts, key);
    if (potentialCount < count) {
        counters_set(andSequence, key, potentialCount);
    }
}

/* ***************** unionCounts ********************** */
/* unions two counters, building up the resulting counters
 */
void unionCounts(void *arg, int key, int count)
{
    counters_t *result = arg;
    
    // add the count for this particular word to the current overall count for the docId
    int currCount = counters_get(result, key);
    counters_set(result, key, currCount + count);
}

/* ***************** validateParameters ********************** */
/* validates there are strictly two command-line arguments:
 * 1. pageDirectory must be a directory created by the crawler
 * 2. indexFilename is a valid readable file created by the index
 */
bool validateParameters(int argc, char *argv[])
{
    // only two command-line arguments
    if (argc != 3) {
        fprintf(stderr, "2 arguments expected (%d received). Usage: ./querier pageDirectory indexFilename\n", argc-1);
        return false;
    }

    // validate that the given directory is a crawler-produced directory (has a readable file .crawler)
    if (!isCrawlerDir(argv[1])) {
        fprintf(stderr, "%s is not a valid crawler-produced directory\n", argv[1]);
        return false;
    }

    // validate that the indexFile is readable
    if (!checkFile("", argv[2], "r")) {
        fprintf(stderr, "%s is not a valid filename\n", argv[2]);
        return false;
    }

    return true;
}

/* ***************** keycount ********************** */
/* counts the number of keys in a given counters
 * for use in counters_iterate
 */
void keycount(void *arg, int key, int count) {
    int *nkeys = arg;
    if (nkeys != NULL && key >= 0 && count >=0) {
        (*nkeys)++;
    }
}

/* ***************** ctrs_delete ********************** */
/* function with a pointer to a void ctrs for use
 * when deleting the inverted index
 */
static void ctrs_delete(void *ctrs)
{
    counters_t *counters = ctrs;
    if (counters != NULL) {
        counters_delete(counters);
    }
}
