/* 
 * quierer.c - 'querier' module for use in the Tiny Search Engine (TSE) project
 *
 * William Dinauer, Dartmouth CS50 Winter 2021
 */
#include <unistd.h>
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
struct twoctrs {                // struct to hold two counters
    counters_t *ctrs1; 
    counters_t *ctrs2;
};

typedef struct pair {           // struct to hold a docId, count pair (as in counters)
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
counters_t *andCounters(counters_t *counts, counters_t *andSequence);
void intersectCounts(void *arg, int key, int count);
void mergeCounters(counters_t *counts, counters_t *result);
void unionCounts(void *arg, int key, int count);
bool isZeros(pair_t *pr);
static void ctrs_delete(void *ctrs);
static void prompt(void);
int fileno(FILE *stream);

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
    // construct the index from the file written by the indexer
    hashtable_t *index = loadIndex(argv[2]);
    if (index == NULL) {
        fprintf(stderr, "unable to load index from the index file; out of memory\n");
        return false;
    }

    prompt(); // prints "Query? " if stdin is from keyboard

    // keep reading until user escapes
    while((line = freadlinep(stdin)) != NULL) {
        int maxWords;
        int len = strlen(line);

        // set maxWords to half the size of the line - the greatest possible number of words in the line
        if (len % 2 == 0){      // even length
            maxWords = len/2;
        } else {                // odd length; add 1 then divide
            maxWords = (len+1)/2;
        }

        // for a line length > 0 
        if (maxWords > 0) {
            int numWords = 0;
            char *words[maxWords];

            // parse the line; write normalized words into words[] and update numWords - the number of words
            if (splitline(line, words, &numWords) && numWords > 0) {
                
                // print the normalized query on a single line
                printf("Query:");
                int i;
                for (i = 0; i < numWords; i++) {
                    printf(" %s", words[i]);
                }
                fputc('\n', stdout);

                // construct the counters holding the docId, score pairs
                counters_t *result = searchMatches(index, words, numWords);
                if (result != NULL) {   // valid syntax, not out of memory

                    // get the number of docIds matched
                    int keyCount = 0;
                    counters_iterate(result, &keyCount, keycount);

                    // print the results
                    if (keyCount == 0) {
                        printf("No documents match.\n");
                    } else {
                        printf("Matches %d documents (ranked):\n", keyCount);
                        // sort and print each docId, score pair
                        printResults(result, keyCount, argv[1]);
                    }
                    counters_delete(result);
                }
            }
        }
        // separate the next query with a bar of repeated '-'
        int x;
        for (x = 0; x < 100; x++) {
            fputc('-', stdout);
        }
        fputc('\n', stdout);

        // print "Query? " if stdin is from a keyboard
        prompt();

        free(line);
        line = NULL;
    }
    // user exiting; clean-up
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
    // create an array of pairs initialzed to {0, 0} for each pair
    const pair_t zeros = {0, 0};
    pair_t pairs[keyCount];
    int i;
    for (i = 0; i < keyCount; i++) {
        pairs[i] = zeros;
    }

    // insert the pairs in the 'result' counters and sort them as they are inserted
    counters_iterate(result, &pairs, sortPairs);

    // print each pair, starting with the docId with the greatest count/score
    for (i = 0; i < keyCount; i++) {
        pair_t *dirCt = &pairs[i];
        int count = dirCt->count;
        int key = dirCt->docId;

        // get the webpage with the associated docId to grab its url
        webpage_t *wp = getNextWebpage(crawlDir, &key);
        char *url = webpage_getURL(wp);
        printf("score: %3d doc %4d: %s\n", count, key, url); // print format
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
    pair_t pr = {key, count}; // construct a pair for this node in the result counters

    int i = 0; // start at position 0
    while (true) {      // keep stepping through the array until we insert our node; 'break' allows us to never need to know the array's length
        pair_t *here = &pairs[i]; // grab the pair at the current position: 'here'
        if (here->count < count) { // if 'here' has a count < this node's count...
            if (isZeros(here)) {    // if 'here' = {0, 0}, simply insert the pair here
                pairs[i] = pr;
                break;
            } else {
                // not at a pair of zeros; we must shift all counts less than this count one position to the right to allow for insertion

                int j = i + 1;  // the position after i (the next position)
                pair_t *next = &pairs[j];   // grab the next pair - 'next'
                while (!isZeros(next)) {    // while the next pair != {0, 0}
                    // store the information of the next pair
                    int nextCount = next->count; 
                    int nextId = next->docId;

                    // move 'here' one position to the right
                    pairs[j] = *here;

                    // update the information of the next pair to 'here'
                    here->count = nextCount;
                    here->docId = nextId;

                    // increment j; grab the next pair
                    j++;               
                    next = &pairs[j];
                }
                // we've reached zeros: update this pair of {0, 0} to the information stored in 'here'
                pairs[j] = *here;

                // we can override whatever was in i; it has been shifted to the right
                pairs[i] = pr;
                break;
            }
        }
        i++;        // increment i to step to the next position in the array
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

    NormalizeWord(line);        // all uppercase letters set to lowercase

    while (true) {
        while (isspace(line[begPos])) {     // move past leading spaces
            begPos++;
        }
        if (line[begPos] == '\0') {         // reached the end of the line; break
            break;
        }
        // we hit a non-space character; the beginning of the word starts here
        beg = &line[begPos];

        // start passing along the endPos pointer
        endPos = begPos;
        while (isalpha(line[endPos])) {  // move past alpha characters, they are part of the word
            endPos++;
        }
        if (line[endPos] == '\0') {     // reached the end of the line; save the pointer to the word in words[]
            words[*numWords] = beg;
            (*numWords)++;
            break;
        }
        if (!isspace(line[endPos])) {  // not an alpha character, not '\0' (end of line), not a space; this must be a bad character
            printf("bad character: '%c' in query\n", line[endPos]);
            return false;
        }
        line[endPos] = '\0';        // update the space to '\0' to indicate the end of a word
        begPos = endPos + 1;        // start looking for a new word again in a position after the newly placed '\0' character
        words[*numWords] = beg;     // save the pointer to words[]
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
    counters_t *result = counters_new();    // the resulting counters containing all docId, score pairs
    counters_t *andSequence = NULL;         // holds the current 'andSequence', which should eventually be merged into result
    if (result == NULL) {   // out of memory
        fprintf(stderr, "out of memory; unable to create 'counters' module\n");
        return NULL;
    }

    bool wasConj = true;        // was the last word read "and" or "or"? bool for syntax check; initialzed to true as we should never start a query with "and" or "or"

    // read through every word in the query
    int i;
    for (i = 0; i < numWords; i++) {
        char *word = words[i];

        // the word is "and"...
        if (isand(word)) {
            if (wasConj || i == numWords - 1) {      // either the previous word was "and" or "or", or this "and" is the last word in the query! This is invalid
                printf("invalid query\n");
                // clean-up; counters_delete will check if andSequence = NULL
                counters_delete(result);
                counters_delete(andSequence);
                return NULL;
            } else {
                // for valid syntax, simply set wasConj to true and keep building up the andSequence by passing over this word
                wasConj = true; 
            }
        } else {
            // the word is "or"...
            if (isor(word)) {
                if (wasConj || i == numWords - 1 ) { // either the previous word was "and" or "or", or this "or" is the last word in the query! This is invalid
                    printf("invalid query\n");
                    counters_delete(result);
                    counters_delete(andSequence);
                    return NULL;
                } else {
                    // valid syntax
                    wasConj = true;

                    // merge the andSequence into the result
                    mergeCounters(andSequence, result);

                    // delete the andSequence and set it to NULL, we should start building up a new andSequence
                    counters_delete(andSequence);
                    andSequence = NULL;
                }
            } else { // for any other word...
                wasConj = false;

                // grab the counters for this word from the index
                counters_t *counts = hashtable_find(index, word);

                // update the andSequence
                 if (andSequence == NULL) {     // is andSequence is NULL...
                    
                     // create a new counters for the current andSequence
                    andSequence = counters_new();
                    if (andSequence == NULL) {  // out of memory
                        fprintf(stderr, "out of memory\n");
                        counters_delete(result);
                        return NULL;
                    }

                    // merge the counts into the empty andSequence (essentially copying the counters from the index)
                    mergeCounters(counts, andSequence);

                 } else {       // otherwise, we are adding to an existing andSequence! So...
                    
                    // intersect the counts with our existing andSequence
                    andSequence = andCounters(andSequence, counts);
                 }
            }
        }
    }
    // we should always have an existing andSequence that's been built up at the end; merge it into the result (union the counters)
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
counters_t *andCounters(counters_t *andSequence, counters_t *counts)
{
    counters_t *result = counters_new();        // new counter to build into
    struct twoctrs ctrs = {result, counts};     // struct to pass as arg

    // intersect the sets, built into 'result'
    counters_iterate(andSequence, &ctrs, intersectCounts);

    // delete the old andSequence; no longer needed
    counters_delete(andSequence);

    return result;      // return the new andSequence
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
    counters_t *result = ctrs->ctrs1;
    counters_t *counts = ctrs->ctrs2;

    // compare the count in 'counts' (from the index) to the current count;
    // keep the minimum
    int potentialCount = counters_get(counts, key);
    if (potentialCount < count) {
        // never store a count of 0
        if (potentialCount != 0) {
            counters_set(result, key, potentialCount);
        }
    } else {
        // no need to check if 0, must already be non-zero
        counters_set(result, key, count);
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

/* ***************** prompt ********************** */
/* taken from CS50 lecture notes on Querier
 *
 * prints the "Query? " prompt if stdin is a keyboard
 */
static void prompt(void)
{
    // print a prompt iff stdin is a tty (terminal)
    if (isatty(fileno(stdin))) {
        printf("Query? ");
    }
}
